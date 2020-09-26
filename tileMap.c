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
#include "headers/tileMap.h"
#include "headers/inventory.h"
#include "headers/mapGeneration.h"


// enum zBufferOrder {RNDRLYR_MAP = 0, RNDRLYR_PLAYER = 5, RNDRLYR_UI = 20, RNDRLYR_INV_ITEMS = 25, RNDRLYR_TEXT = 30};

const int MAXLEVELSIZE = 512;

LevelComponent *activeLevel;

int tilePixelSize = 16;
const int tileStretchSize = 64;
int tileSheetWidth = 8;

LevelComponent *levels;
int numLevels = -1;

BlockComponent undefinedBlock;
BlockComponent *blockData;
int numBlocks = -1;

AutotileComponent *autotileData;
int numAutotiles = -1;

/*Formula

x = (tileNum % tileSheetWidth) * 16
y = (tileNum / tileSheetHeight) * 16

*/
int renderItemIndex = 0;
RenderComponent *renderBuffer;

void SetupRenderFrame(){//Clear and allocate render buffer + reset render counter
	free(renderBuffer);
	renderItemIndex = 0;
	renderBuffer = malloc(sizeof(RenderComponent));
}

int AddToRenderQueue(SDL_Renderer *renderer, WB_Tilesheet *tileSheet, int tileNum, SDL_Rect destRect, int alpha, int zPos){
	return AddToRenderQueueEx(renderer, tileSheet, tileNum, destRect, alpha, zPos, 0);
}

int AddToRenderQueueEx(SDL_Renderer *renderer, WB_Tilesheet *tileSheet, int tileNum, SDL_Rect destRect, int alpha, int zPos, int rotation){
	// if(tileSheet->tex == NULL){
	// 	printf("Error: Tilesheet not defined properly!\n");
	// 	return 1;
	// }
	if(alpha == -1){
		alpha = 255;
	}
	if(tileNum <= tileSheet->w * tileSheet->h - 1){
		renderBuffer = realloc(renderBuffer, (renderItemIndex + 1) * sizeof(RenderComponent));
		renderBuffer[renderItemIndex] = (RenderComponent){renderer, tileSheet, tileNum, destRect, alpha, zPos, rotation};
		renderItemIndex++;
		return 0;
	}else if(strcmp(tileSheet->name, "undefined") == 0 || tileSheet->tex == NULL || tileNum < 0){
		renderBuffer = realloc(renderBuffer, (renderItemIndex + 1) * sizeof(RenderComponent));
		renderBuffer[renderItemIndex] = (RenderComponent){renderer, &undefinedSheet, 0, destRect, 255, zPos, 0};
		renderItemIndex++;
	}else{
		printf("Error: Tile index not in image bounds!\n");
		return 1;
	}
}

// int RenderTextureFromSheet(){
void RenderUpdate(){
	int key, j; //Insertion sort
	RenderComponent tmpRenderItem;
	for (int i = 1; i < renderItemIndex; i++) {
		tmpRenderItem = renderBuffer[i];
		key = renderBuffer[i].zPos;
		j = i - 1;
		while (j >= 0 && renderBuffer[j].zPos > key) {
			renderBuffer[j + 1] = renderBuffer[j];
			j--;
		}
		renderBuffer[j + 1] = tmpRenderItem;
	}
	
	Vector2 tileInSheet;
	for(int i = 0; i < renderItemIndex; i++){
		tileInSheet.x = (renderBuffer[i].tile % renderBuffer[i].tileSheet->w) * renderBuffer[i].tileSheet->tile_size;
		tileInSheet.y = (renderBuffer[i].tile / renderBuffer[i].tileSheet->w) * renderBuffer[i].tileSheet->tile_size;
		
		SDL_SetTextureAlphaMod(renderBuffer[i].tileSheet->tex, renderBuffer[i].alpha);
		SDL_Rect sourceRect = {tileInSheet.x, tileInSheet.y, renderBuffer[i].tileSheet->tile_size, renderBuffer[i].tileSheet->tile_size};
		SDL_RenderCopyEx(renderBuffer[i].renderer, renderBuffer[i].tileSheet->tex, &sourceRect, &renderBuffer[i].transform, renderBuffer[i].rotation * 90, NULL, SDL_FLIP_NONE);	
	}
	SetupRenderFrame();
}


BlockComponent *find_block(char *name){
	for(int i = 0; i < numBlocks + 1; i++){
		if(strcmp(name, blockData[i].item->name) == 0){
			return &blockData[i];
		}
	}
	return &undefinedBlock;
}

int register_block(lua_State *L){
	numBlocks++;
	blockData = realloc(blockData, (numBlocks + 10) * sizeof(BlockComponent));

	luaL_checktype(L, 1, LUA_TTABLE);

	lua_getfield(L, -1, "name");
	if(strcmp(find_item((char *)lua_tostring(L, -1))->name, "undefined") == 0){//Check if the item already exists
		numItems++;
		itemData = realloc(itemData, (numItems + 1) * sizeof(ItemComponent));

		if(lua_tostring(L, -1) != NULL && strlen(lua_tostring(L, -1)) > 0){
			itemData[numItems].name = malloc(sizeof(char) * (strlen(lua_tostring(L, -1)) + 1));
			strcpy(itemData[numItems].name, lua_tostring(L, -1));
		}else{
			itemData[numItems].name = malloc(sizeof(char) * (strlen("undefined") + 1));
			strcpy(itemData[numItems].name, "undefined");
		}

		lua_getfield(L, -2, "item_sheet");
		if(lua_tostring(L, -1) != NULL){
			itemData[numItems].sheet = *find_tilesheet((char *)lua_tostring(L, -1));
		}else{
			itemData[numItems].sheet = undefinedSheet;
		}

		lua_getfield(L, -3, "item_tile_index");
		if(lua_tonumber(L, -1)){
			itemData[numItems].tile = lua_tonumber(L, -1);
		}else{
			itemData[numItems].tile = 0;
		}

		blockData[numBlocks].item = &itemData[numItems];
		itemData[numItems].isBlock = true;


		// char *log;
		// sprintf(log, "Created block '%s'", blockData[numBlocks].item->name);
		// sprintf(log, "Created block '%s'", itemData[numItems].name);
		// DebugLog(D_SCRIPT_ACT, log);
		// char *log = malloc(strlen(itemData[numItems].name) + 32);
		// sprintf(log, "Created block '%s'", itemData[numItems].name);
		DebugLog(D_SCRIPT_ACT, "Created block '%s'", itemData[numItems].name);
		// free(log);
	}else{//If it does, use it
		blockData[numBlocks].item = find_item((char *)lua_tostring(L, -1));
		find_item((char *)lua_tostring(L, -1))->isBlock = true;
	}
	
	lua_getfield(L, -4, "block_sheet");
	if(lua_tostring(L, -1) != NULL){
		blockData[numBlocks].sheet = *find_tilesheet((char *)lua_tostring(L, -1));
	}else{
		blockData[numBlocks].sheet = undefinedSheet;
	}

	lua_getfield(L, -5, "block_tile_index");
	if(lua_tonumber(L, -1) || lua_tonumber(L, -1) == 0){
		blockData[numBlocks].tile = lua_tonumber(L, -1);
	}else{
		blockData[numBlocks].tile = -1;
	}

	lua_getfield(L, -6, "dropped_item");
	if(lua_tostring(L, -1) != NULL){
		if(find_item((char *)lua_tostring(L, -1)) != NULL){
			blockData[numBlocks].dropItem = find_item((char *)lua_tostring(L, -1));
		}else{
			blockData[numBlocks].dropItem = blockData[numBlocks].item;
		}
	}else{
		blockData[numBlocks].dropItem = blockData[numBlocks].item;
	}

	lua_getfield(L, -7, "dropped_qty");
	if(lua_tonumber(L, -1)){
		blockData[numBlocks].dropQty = lua_tonumber(L, -1);
	}else{
		blockData[numBlocks].dropQty = 1;
	}

	// lua_pop(L, 1);
	lua_getfield(L, -8, "collision_type");
	if(lua_toboolean(L, -1) != NULL){
		blockData[numBlocks].collisionType = lua_tonumber(L, -1);
	}else{
		blockData[numBlocks].collisionType = -1;
	}
	// printf("%d\n", blockData[numBlocks].enableCollision);


	lua_getfield(L, -9, "block_layer");
	if(lua_tostring(L, -1) != NULL){
		strcpy(blockData[numBlocks].layer, lua_tostring(L, -1));
	}else{
		strcpy(blockData[numBlocks].layer, "terrain");
	}

	if(strcmp(blockData[numBlocks].layer, "terrain") == 0 && blockData[numBlocks].collisionType > 0){
		blockData[numBlocks].collisionType = -1;
	}

	lua_getfield(L, -10, "enable_rotation");
	if(lua_toboolean(L, -1) != NULL){
		blockData[numBlocks].allowRotation = lua_toboolean(L, -1);
	}else{
		blockData[numBlocks].allowRotation = false;
	}

	// blockData[numBlocks].flags = malloc(sizeof(char **));
	// lua_getfield(L, -8, "flags");
	// if(lua_istable(L, -1)){
	// 	size_t len = lua_rawlen(L, -1);
	// 	int numFlags = 0;
	// 	for(int i = 0; i <= len; i++){
	// 		lua_pushinteger(L, i + 1);
	// 		lua_gettable(L, -2);	
	// 		if(lua_tostring(L, -1) != NULL){
	// 			// char *temp = calloc(strlen(lua_tostring(L, -1)), sizeof(char));
	// 			// strcpy(temp, lua_tostring(L, -1));
	// 			blockData[numBlocks].flags[numFlags] = calloc(strlen(lua_tostring(L, -1)) + 1, sizeof(char));
	// 			// printf("%d   ", strlen(lua_tostring(L, -1)));
	// 			strcpy(blockData[numBlocks].flags[numFlags], lua_tostring(L, -1));
	// 			// blockData[numBlocks].flags[numFlags][strlen(blockData[numBlocks].flags[numFlags])] = '\0';
	// 			// strcpy(blockData[numBlocks].flags[numFlags], temp);

	// 			printf("%s\n", blockData[numBlocks].flags[numFlags]);
	// 			numFlags++;
	// 		}
	// 		lua_pop(L, 1);
	// 	}
	// }
	

	// blockData[numBlocks].autoTile = false;

	return 0;
}

int populate_autotile(lua_State *L){
	numAutotiles++;
	autotileData = realloc(autotileData, (numAutotiles + 1) * sizeof(AutotileComponent));

	luaL_checktype(L, 1, LUA_TTABLE);

	//Get name
	lua_getfield(L, -1, "name");
	if(lua_tostring(L, -1) != NULL && strlen(lua_tostring(L, -1)) > 0){
		autotileData[numAutotiles].name = calloc(strlen(lua_tostring(L, -1)), sizeof(char));
		strcpy(autotileData[numAutotiles].name, lua_tostring(L, -1));
	}else{//Undefined
		autotileData[numAutotiles].name = calloc(strlen("undefined"), sizeof(char));
		strcpy(autotileData[numAutotiles].name, "undefined");
	}

	//Get base item
	lua_getfield(L, -2, "base_block");
	if(lua_tostring(L, -1) != NULL && strlen(lua_tostring(L, -1)) > 0){
		autotileData[numAutotiles].baseBlock = find_block((char *)lua_tostring(L, -1));
	}else{//Undefined
		autotileData[numAutotiles].baseBlock = &undefinedBlock;
	}

	//Get sub item
	lua_getfield(L, -3, "sub_block");
	if(lua_tostring(L, -1) != NULL && strlen(lua_tostring(L, -1)) > 0){
		autotileData[numAutotiles].subBlock = find_block((char *)lua_tostring(L, -1));
	}else{//Undefined
		autotileData[numAutotiles].subBlock = &undefinedBlock;
	}

	//For loop to loop between the base and sub
	//Setup blend modes
	SDL_SetTextureBlendMode(autotileMaskTex, SDL_BLENDMODE_ADD);
	SDL_SetTextureBlendMode(InvertedAutotileMaskTex, SDL_BLENDMODE_ADD);
	SDL_SetTextureBlendMode(autotileData[numAutotiles].baseBlock->sheet.tex, SDL_BLENDMODE_ADD);
	SDL_SetTextureBlendMode(autotileData[numAutotiles].subBlock->sheet.tex, SDL_BLENDMODE_ADD);
	SDL_Texture *tmpTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, tilePixelSize, tilePixelSize);//Texture with masked baseTile
	SDL_SetTextureBlendMode(tmpTex, SDL_BLENDMODE_MOD);
	for(int i = 1; i <= 46; i++){
		autotileData[numAutotiles].auto_block = realloc(autotileData[numAutotiles].auto_block, (i + 1) * sizeof(BlockComponent_local));
		autotileData[numAutotiles].auto_block[i - 1] = *autotileData[numAutotiles].baseBlock;
		autotileData[numAutotiles].auto_block[i - 1].autoTile = true;
		autotileData[numAutotiles].auto_block[i - 1].tile = 0;
		// autotileData[numAutotiles].auto_block[i - 1].id = i;

		//Mask method

		autotileData[numAutotiles].auto_block[i - 1].sheet.tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, tilePixelSize, tilePixelSize);
		
		//Create mask from baseTile texture
		SDL_SetRenderTarget(renderer, tmpTex);
		SDL_RenderClear(renderer);

		int baseTile = autotileData[numAutotiles].baseBlock->tile;
		SDL_Rect baseRect = {(baseTile % autotileData[numAutotiles].baseBlock->sheet.w) * tilePixelSize, (baseTile / autotileData[numAutotiles].baseBlock->sheet.w) * tilePixelSize, tilePixelSize, tilePixelSize};
		SDL_RenderCopy(renderer, autotileData[numAutotiles].baseBlock->sheet.tex, &baseRect, NULL);
		
		SDL_Rect maskRect = {(i % autotileData[numAutotiles].baseBlock->sheet.w) * tilePixelSize, (i / autotileData[numAutotiles].baseBlock->sheet.w) * tilePixelSize, tilePixelSize, tilePixelSize};
		SDL_RenderCopy(renderer, autotileMaskTex, &maskRect, NULL);

		//Mask out subTile and render tmpTex onto autotile-id texture
		SDL_SetRenderTarget(renderer, autotileData[numAutotiles].auto_block[i - 1].sheet.tex);
		SDL_RenderClear(renderer);

		int subTile = autotileData[numAutotiles].subBlock->tile;
		SDL_Rect subRect = {(subTile % autotileData[numAutotiles].subBlock->sheet.w) * tilePixelSize, (subTile / autotileData[numAutotiles].subBlock->sheet.w) * tilePixelSize, tilePixelSize, tilePixelSize};
		SDL_RenderCopy(renderer, autotileData[numAutotiles].subBlock->sheet.tex, &subRect, NULL);
		
		SDL_RenderCopy(renderer, InvertedAutotileMaskTex, &maskRect, NULL);
		
		SDL_RenderCopy(renderer, tmpTex, NULL, NULL);//Render the final masked baseTile onto the masked subTile
	}
	//Reset any changes and destroy texture
	SDL_DestroyTexture(tmpTex);
	SDL_SetRenderTarget(renderer, NULL);
	SDL_SetTextureBlendMode(autotileData[numAutotiles].baseBlock->sheet.tex, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(autotileData[numAutotiles].subBlock->sheet.tex, SDL_BLENDMODE_BLEND);

	return 0;
}




int LoadMap(char *fileLoc, RenderTileComponent mapArray[][32]){
	FILE *fp = fopen(fileLoc, "r");
	
	if(fp == NULL){
		printf("Error: Unable to open mapfile location: '%s'\n", fileLoc);
		return 1;
	}
	
	char buffer[256];
	char *parsedInt;
	// printf("%s\n", fileLoc);
	for(int y = 0; y <= 31; y++){
		
		fgets(buffer, sizeof(buffer) / sizeof(buffer[0]), fp);
		parsedInt = strtok(buffer, ",");
		
		for(int x = 0; x <= 31; x++){
			mapArray[y][x].block = malloc(sizeof(BlockComponent));
			mapArray[y][x].type = strtol(parsedInt, NULL, 10);
			if(strtol(parsedInt, NULL, 10) > numBlocks || strtol(parsedInt, NULL, 10) == -1){
				mapArray[y][x].block = &undefinedBlock;
			}else{
				mapArray[y][x].block = &blockData[strtol(parsedInt, NULL, 10)];
				// printf("%d\n", mapArray[y][x].block->tile);
				// printf("%d\n", strtol(parsedInt, NULL, 10) - 1);
			}
			parsedInt = strtok(NULL, ",");
			// printf("%s", parsedInt);
			// printf("%d", mapArray[y][x].type);
		}
		// printf("\n");
	}
	fclose(fp);
	return 0;
}

int LoadDataMap(char *fileLoc, int mapArray[][32]){//For non rendered maps (e.g. collision)
	FILE *fp = fopen(fileLoc, "r");
	
	if(fp == NULL){
		printf("Error: Unable to open mapfile location: '%s'\n", fileLoc);
		return 1;
	}
	
	char buffer[256];
	char *parsedInt;
	// printf("%s\n", fileLoc);
	for(int y = 0; y <= 31; y++){
		
		fgets(buffer, sizeof(buffer) / sizeof(buffer[0]), fp);
		parsedInt = strtok(buffer, ",");
		
		for(int x = 0; x <= 31; x++){
			mapArray[y][x] = strtol(parsedInt, NULL, 10);
			parsedInt = strtok(NULL, ",");
			// printf("%s", parsedInt);
			// printf("%d", mapArray[y][x]);
		}
		// printf("\n");
	}
	fclose(fp);
	return 0;
}

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

			// printf("%s\n", lineBuffer);
			/*
			Check for block rotation specifier using strchr to find ~
			If ~ exists strtok once more to seperate block name and rotation specifier
			strtol rotation specifier
			check if block supports rotation, if so assign it to block.rotation
			*/
			if(lineBuffer[0] == ':' && lineBuffer[1] == ':'){//Detect headers
				strcpy(header, strshft_l(lineBuffer, 2));
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
		// strcpy(levels[numLevels].name, strrchr(strtok(path, "."), '/') != NULL ? strrchr(strtok(path, "."), '/') + 1 : strtok(path, "."));
		if(strrchr(strtok(path, "."), '/') != NULL){
			strcpy(levels[numLevels].name, strrchr(strtok(path, "."), '/') + 1);
		}else{
			strcpy(levels[numLevels].name, strtok(path, "."));
		}
	}
	fclose(level);
	DefineCollisions(&levels[numLevels]);
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
	fprintf(file, "seed:%u\n", level->seed);
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
	// DrawMap(*find_tilesheet("default_ground"), levels[0].terrain, 0);
	// DrawMap(*find_tilesheet("furniture"), levels[0].features, 1);

	// RenderLevel(&levels[0]);
	// RenderLevel(&levels[0]);
	RenderLevel(activeLevel);

	// RenderLevel(&levels[1]);

	DrawCharacter(characterFacing, 6);
}