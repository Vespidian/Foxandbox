#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "headers/DataTypes.h"
#include "headers/ECS.h"
#include "headers/initialize.h"
#include "headers/data.h"
#include "headers/level_systems.h"
#include "headers/render_systems.h"
#include "headers/inventory.h"
#include "headers/map_generation.h"
#include "headers/lua_systems.h"

const int MAXLEVELSIZE = 512;

LevelComponent *activeLevel;

LevelComponent *levels;
int numLevels = -1;

RenderTileComponent **mouseEditingLayer;

DroppedItemComponent *droppedItems;
int numDroppedItems = 0;

int GetLineLength(FILE *file){
	int lineLength = 0;
	while(fgetc(file) != '\n'){
		lineLength++;
	}
	fseek(file, -lineLength - 2, SEEK_CUR);
	return lineLength * 8;
}

int LoadLevel(char *path){
	/*
	Check if path exists
	Load file with read permissions
	Read level dimensions and allocate level array
	Look for header of section (Terrain:, Features:, Collision:, etc)
	Read data to coresponding array
	*/

	FILE *level = fopen(path, "r");
	if(level != NULL){//Check if path exists
		numLevels++;
		levels = realloc(levels, (numLevels + 1) * sizeof(LevelComponent));
		char header[64];
		char *lineBuffer;
		int lineLength = GetLineLength(level);
		int line = 0;
		int y = 0;
		Vector2 mapSize = {0, 0};
		lineBuffer = malloc(lineLength);
		while(fgets(lineBuffer, lineLength, level)){
			if(strcmp(header, "config") == 0 && lineBuffer[0] != ':' && lineBuffer[1] != ':'){
				char *token;
				char *data;
				token = strtok(lineBuffer, ":");
				data = strtok(NULL, "\n");
				if(strcmp(token, "size") == 0){
					mapSize.x = strtol(strtok(data, "x"), NULL, 10);
					mapSize.y = strtol(strtok(NULL, "\n"), NULL, 10);
					if(mapSize.x > MAXLEVELSIZE){
						mapSize.x = MAXLEVELSIZE;
					}
					if(mapSize.y > MAXLEVELSIZE){
						mapSize.y = MAXLEVELSIZE;
					}
					levels[numLevels].map_size = mapSize;
					InitializeBlankLevel(&levels[numLevels], levels[numLevels].map_size);

				}else if(strcmp(token, "seed") == 0){
					levels[numLevels].seed = strtol(data, NULL, 10);
				}
			}

			if(strchr(lineBuffer, '\n') != NULL){
				*strchr(lineBuffer, '\n') = '\0';
			}
			/*
			Check for block rotation specifier using strchr to find ~
			If ~ exists strtok once more to seperate block name and rotation specifier
			strtol rotation specifier
			check if block supports rotation, if so assign it to block.rotation
			*/
			if(lineBuffer[0] == ':' && lineBuffer[1] == ':'){//Detect headers
				// strcpy(header, strshft_l(lineBuffer, 2));//MINGW BINARY CRASHES ON THIS LINE
				strshft_l(lineBuffer, 2);
				strcpy(header, lineBuffer);
				y = 0;
			}else if(y < mapSize.y){	
				if(strcmp(header, "terrain") == 0){
					char token[128];
					strcpy(token, strtok(lineBuffer, ","));
					for(int x = 0; x < mapSize.x; x++){
						if(strchr(token, '~') != NULL){
							int rotVal = strtol(strchr(token, '~') + 1, NULL, 10);
							*(strchr(token, '~')) = '\0';
							if(find_block(token)->allowRotation){
								levels[numLevels].terrain[y][x].rotation = rotVal;
							}
						}
						levels[numLevels].terrain[y][x].block = find_block(token);

						// printf("%s\n", token);
						char *tmp = strtok(NULL, ",");
						if(tmp != NULL){
							strcpy(token, tmp);
						}else{
							strcpy(token, "undefined");
						}
					}
					y++;
				}else if(strcmp(header, "features") == 0){
					char token[128];
					strcpy(token, strtok(lineBuffer, ","));
					for(int x = 0; x < mapSize.x; x++){
						if(strchr(token, '~') != NULL){
							int rotVal = strtol(strchr(token, '~') + 1, NULL, 10);
							*(strchr(token, '~')) = '\0';
							if(find_block(token)->allowRotation){
								levels[numLevels].features[y][x].rotation = rotVal;
							}
						}
						levels[numLevels].features[y][x].block = find_block(token);

						// printf("%s\n", token);
						char *tmp = strtok(NULL, ",");
						if(tmp != NULL){
							strcpy(token, tmp);
						}else{
							strcpy(token, "undefined");
						}
					}
					y++;
				}
			}

			lineLength = GetLineLength(level);
			lineBuffer = malloc(lineLength);
			line++;
		}
		free(lineBuffer);
		levels[numLevels].name = malloc(strlen(strtok(path, ".")) * sizeof(char));
		if(strrchr(strtok(path, "."), '/') != NULL){
			strcpy(levels[numLevels].name, strrchr(strtok(path, "."), '/') + 1);
		}else{
			strcpy(levels[numLevels].name, strtok(path, "."));
		}
	}
	fclose(level);
	DefineCollisions(&levels[numLevels]);
	// printf("%s\n", levels[numLevels].terrain[0][0].block->item->name);
	return 0;
}

int SaveLevel(LevelComponent *level, char *path){
	/*
	Check if path exists, if not create it
	Load file with write permissions

	loop:
	Write header for map array type
	Go through map arrays (Terrain:, Features:, Collision:, etc) and copy each row to a string
	fprintf string to file and go to newline
	goto loop for each layer type

	Write additional data such as portal/door locations

	free file
	*/
	FILE *file = fopen(path, "w");

	fprintf(file, "::config\n");
	fprintf(file, "size:%dx%d\n", level->map_size.x, level->map_size.y);
	fprintf(file, "seed:%lu\n", level->seed);
	fprintf(file, "::terrain\n");
	for(int y = 0; y < level->map_size.y; y++){
		for(int x = 0; x < level->map_size.x; x++){
			fprintf(file, "%s", level->terrain[y][x].block->item->name);
			if(level->terrain[y][x].block->allowRotation){
				fprintf(file, "~%d", level->terrain[y][x].rotation);
			}

			fprintf(file, ",");
		}
		fprintf(file, "\n");
	}

	fprintf(file, "::features\n");
	for(int y = 0; y < level->map_size.y; y++){
		for(int x = 0; x < level->map_size.x; x++){
			fprintf(file, "%s", level->features[y][x].block->item->name);
			if(level->features[y][x].block->allowRotation){
				fprintf(file, "~%d", level->features[y][x].rotation);
			}

			fprintf(file, ",");
		}
		fprintf(file, "\n");
	}

	fprintf(file, "\n\n");
	fclose(file);
	return 0;
}

int UnloadLevel(LevelComponent *level){
	/*
	Make sure the specified level is not the current active level
	Free both dimensions of layer arrays
	Free entire level
	Decrement numLevels counter
	*/
	if(&level != &activeLevel){
		for(int y = 0; y < level->map_size.y; y++){
			free(level->collision[y]);
			free(level->features[y]);
			free(level->terrain[y]);
		}
		level->map_size.x = 0;
		level->map_size.y = 0;

		free(level->collision);
		free(level->features);
		free(level->terrain);
		free(level);
		numLevels--;
	}
	return 0;
}

void RenderLevel(LevelComponent *level){//Draw map from 2D array
	Vector2 tilePos = {0, 0};
	SDL_Rect tile = {tilePos.x, tilePos.y, tileStretchSize, tileStretchSize};
	AddToRenderQueue(renderer, find_tilesheet("ui"), 5, mapRect, 255, 0);
	for(int y = (mapOffsetPos.y / tileSize - 1) * ((mapOffsetPos.y / tileSize - 1) > 0); y < ((mapOffsetPos.y + HEIGHT) / tileSize + 1) && y < level->map_size.y; y++){
		for(int x = (mapOffsetPos.x / tileSize - 1) * ((mapOffsetPos.x / tileSize - 1) > 0); x < ((mapOffsetPos.x + WIDTH) / tileSize + 1) && x < level->map_size.x; x++){
			tilePos.x = (x * tileStretchSize) - mapOffsetPos.x;
			tilePos.y = (y * tileStretchSize) - mapOffsetPos.y;
			tile.x = tilePos.x;
			tile.y = tilePos.y;
			// AddToRenderQueue(renderer, find_tilesheet("blocks"), 4, tile, 255, 0);
			if(level->terrain[y][x].type != -1){//Render Terrain
				AddToRenderQueueEx(renderer, level->terrain[y][x].block->sheet, level->terrain[y][x].block->tile, tile, -1, level->terrain[y][x].zPos, level->terrain[y][x].rotation, (SDL_Color){255, 255, 255});
				level->terrain[y][x].zPos = 0;
			}
			if(level->features[y][x].type != -1 && level->terrain[y][x].type != -1){//Render Features
				AddToRenderQueueEx(renderer, level->features[y][x].block->sheet, level->features[y][x].block->tile, tile, -1, level->features[y][x].zPos, level->features[y][x].rotation, (SDL_Color){255, 255, 255});
				level->features[y][x].zPos = 1;
			}
		}
	}
}

void DrawLevel(){
	RenderLevel(activeLevel);
	DrawCharacter(characterFacing, 6);
}

void RenderCursor(){// Highlight the tile the mouse is currently on
	SDL_Rect mapRect = {-mapOffsetPos.x, -mapOffsetPos.y, 64 * activeLevel->map_size.x, 64 * activeLevel->map_size.y};
	// mouseTransform.tilePos = (Vector2){mouseTransform.screenPos.x, mouseTransform.screenPos.y};
	mouseTransform.tilePos.x = ((mouseTransform.screenPos.x + mapOffsetPos.x) / 64);
	mouseTransform.tilePos.y = ((mouseTransform.screenPos.y + mapOffsetPos.y) / 64);
	SDL_Point cursor = {(mouseTransform.tilePos.x * 64) - mapOffsetPos.x, (mouseTransform.tilePos.y * 64) - mapOffsetPos.y};
	if(SDL_PointInRect(&cursor, &mapRect) && !uiInteractMode && !gamePaused){
		if((abs(character.transform.tilePos.x - mouseTransform.tilePos.x) <= reachDistance && abs(character.transform.tilePos.y - mouseTransform.tilePos.y) <= reachDistance) || !reachLimit){
		//Determine wether or not the user can reach infinitely
			AddToRenderQueue(renderer, find_tilesheet("ui"), 3, (SDL_Rect){cursor.x, cursor.y, 64, 64}, -1, RNDRLYR_UI - 10);
			
			//MouseText
			if(showDebugInfo){
				char screenPosT[256];
				snprintf(screenPosT, 1024, "mouseTransform.screenPos ->\nx: %d, y: %d", mouseTransform.tilePos.x, mouseTransform.tilePos.y);
				RenderText_d(renderer, screenPosT, 0, 96);
			}

			if(mouseHeld){//Place and remove tiles
				Vector2 tile = {mouseTransform.tilePos.x, mouseTransform.tilePos.y};
				//Only place the item if it is a block and the selected hotbar is occupied
				//Only place if the indicated block is different from the selected hotbar block
				if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)){
					if(invArray[selectedHotbar].occupied && invArray[selectedHotbar].item->isBlock){
						if(&invArray[selectedHotbar].item->name != &mouseEditingLayer[tile.y][tile.x].block->item->name){
							//If the player is where the block is to be placed, only place it if its non collidable
							if((!SDL_HasIntersection(&character.collider.boundingBox, &(SDL_Rect){(tile.x * 64) - mapOffsetPos.x, (tile.y * 64) - mapOffsetPos.y, 64, 64}) || find_block(invArray[selectedHotbar].item->name)->collisionType != 0)){
								INV_Add(mouseEditingLayer[tile.y][tile.x].block->dropQty, mouseEditingLayer[tile.y][tile.x].block->dropItem, -1);
								INV_WriteCell("sub", selectedHotbar, 1, invArray[selectedHotbar].item);
								PlaceBlock(mouseEditingLayer, tile, find_block(invArray[selectedHotbar].item->name));
							}
						}
					}
				}else if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)){
					if(mouseEditingLayer[tile.y][tile.x].block != find_block("air")){
						DebugLog(D_ACT, "Removed tile '%s' at %d,%d", mouseEditingLayer[tile.y][tile.x].block->item->name, tile.x, tile.y);
						INV_Add(activeLevel->terrain[tile.y][tile.x].block->dropQty, mouseEditingLayer[tile.y][tile.x].block->dropItem, -1);
						mouseEditingLayer[tile.y][tile.x].block = find_block("air");
						if(activeLevel->features[tile.y][tile.x].block == find_block("air")){
							activeLevel->collision[tile.y][tile.x] = -1;
						}else{
							activeLevel->collision[tile.y][tile.x] = activeLevel->features[tile.y][tile.x].block->collisionType;
						}
					}
				}
			}
		}
	}
}

void DropItem(ItemComponent *item, int qty, Vector2 pos){
	if(qty > 0){
		if(activeLevel->collision[mouseTransform.tilePos.y][mouseTransform.tilePos.x] != 0 && SDL_PointInRect((SDL_Point *)&mouseTransform.worldPos, &mapRect)){
			droppedItems = realloc(droppedItems, (numDroppedItems + 1) * sizeof(DroppedItemComponent));

			droppedItems[numDroppedItems].item = item;
			droppedItems[numDroppedItems].qty = qty;
			droppedItems[numDroppedItems].transform.worldPos = (Vector2)pos;
			droppedItems[numDroppedItems].animLocation = 0;
			droppedItems[numDroppedItems].animDir = 0;

			numDroppedItems++;
		}
	}
}

void RenderDroppedItems(){
	for(int i = 0; i < numDroppedItems; i++){
		SDL_Rect itemRect = {droppedItems[i].transform.worldPos.x - mapOffsetPos.x, 
			droppedItems[i].transform.worldPos.y - mapOffsetPos.y + droppedItems[i].animLocation,
			32, 32};
		SDL_Rect winRect = {-tileSize, -tileSize, WIDTH + tileSize, HEIGHT + tileSize};
		SDL_Point p = {itemRect.x, itemRect.y};
		if(SDL_PointInRect(&p, &winRect)){
			if(droppedItems[i].animDir == 1){
				droppedItems[i].animLocation -= 0.6;
			}else if(droppedItems[i].animDir == 0){
				droppedItems[i].animLocation += 0.6;
			}
			if(droppedItems[i].animLocation >= 10){
				droppedItems[i].animDir = 1;
			}else if(droppedItems[i].animLocation <= 0){
				droppedItems[i].animDir = 0;
			}

			if(SDL_HasIntersection(&character.collider.boundingBox, &itemRect)){
				INV_Add(droppedItems[i].qty, droppedItems[i].item, -1);

				numDroppedItems--;
				for(int j = i; j < numDroppedItems; j++){
					droppedItems[j] = droppedItems[j + 1];
				}
				droppedItems = realloc(droppedItems, (numDroppedItems + 1) * sizeof(DroppedItemComponent));
			}
			AddToRenderQueue(renderer, droppedItems[i].item->sheet, droppedItems[i].item->tile, itemRect, 255, RNDRLYR_PLAYER + 5);
		}
	}
}