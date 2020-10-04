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
				strshft_l(lineBuffer, 2);//MINGW BINARY CRASHES ON THIS LINE
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
		levels[numLevels].name = malloc(strlen(strtok(path, ".")) * sizeof(char));
		if(strrchr(strtok(path, "."), '/') != NULL){
			strcpy(levels[numLevels].name, strrchr(strtok(path, "."), '/') + 1);
		}else{
			strcpy(levels[numLevels].name, strtok(path, "."));
		}
	}
	fclose(level);
	DefineCollisions(&levels[numLevels]);
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
	AddToRenderQueue(renderer, &debugSheet, 4, (SDL_Rect){-mapOffsetPos.x, -mapOffsetPos.y, WIDTH + mapOffsetPos.x, HEIGHT + mapOffsetPos.y}, 255, 0);
	for(int y = (mapOffsetPos.y / tileSize - 1) * ((mapOffsetPos.y / tileSize - 1) > 0); y < ((mapOffsetPos.y + HEIGHT) / tileSize + 1) && y < level->map_size.y; y++){
		for(int x = (mapOffsetPos.x / tileSize - 1) * ((mapOffsetPos.x / tileSize - 1) > 0); x < ((mapOffsetPos.x + WIDTH) / tileSize + 1) && x < level->map_size.x; x++){
			tilePos.x = (x * tileStretchSize) - mapOffsetPos.x;
			tilePos.y = (y * tileStretchSize) - mapOffsetPos.y;
			tile.x = tilePos.x;
			tile.y = tilePos.y;
			// AddToRenderQueue(renderer, find_tilesheet("blocks"), 4, tile, 255, 0);
			if(level->terrain[y][x].type != -1){//Render Terrain
				AddToRenderQueueEx(renderer, &level->terrain[y][x].block->sheet, level->terrain[y][x].block->tile, tile, -1, level->terrain[y][x].zPos, level->terrain[y][x].rotation);
				level->terrain[y][x].zPos = 0;
			}
			if(level->features[y][x].type != -1 && level->terrain[y][x].type != -1){//Render Features
				AddToRenderQueueEx(renderer, &level->features[y][x].block->sheet, level->features[y][x].block->tile, tile, -1, level->features[y][x].zPos, level->features[y][x].rotation);
				level->features[y][x].zPos = 1;
			}
		}
	}
}

void DrawLevel(){
	RenderLevel(activeLevel);
	DrawCharacter(characterFacing, 6);
}