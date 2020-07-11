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


int tilePixelSize = 16;
const int tileStretchSize = 64;
int tileSheetWidth = 8;


int colMap[32][32] = {};
RenderTileComponent map[32][32] = {};
RenderTileComponent map1[32][32] = {};
RenderTileComponent furnitureMap[32][32] = {};
RenderTileComponent customMap[32][32] = {};

RenderTileComponent buildLayer[32][32] = {};

/*Formula

x = (tileNum % tileSheetWidth) * 16
y = (tileNum / tileSheetHeight) * 16

*/
int renderItemIndex = 0;
RenderComponent *renderBuffer;

void SetupRenderFrame(){//Clear and allocate render buffer + reset render counter
	// printf("%d\n", renderItemIndex);
	free(renderBuffer);
	renderItemIndex = 0;
	renderBuffer = malloc(sizeof(RenderComponent));
}

int AddToRenderQueue(SDL_Renderer *gRenderer, WB_Tilesheet tileSheet, int tileNum, SDL_Rect destRect, int alpha, int zPos){
	// if(tileSheet.tex == NULL){
	// 	printf("Error: Tilesheet not defined properly!\n");
	// 	return 1;
	// }
	if(alpha == -1){
		alpha = 255;
	}
	if(tileNum <= tileSheet.w * tileSheet.h - 1){
		renderBuffer = realloc(renderBuffer, (renderItemIndex + 1) * sizeof(RenderComponent));
		renderBuffer[renderItemIndex] = (RenderComponent){gRenderer, tileSheet, tileNum, destRect, alpha, zPos};
		renderItemIndex++;
		return 0;
	}else if(strcmp(tileSheet.name, "undefined") == 0 || tileSheet.tex == NULL){
		renderBuffer = realloc(renderBuffer, (renderItemIndex + 1) * sizeof(RenderComponent));
		renderBuffer[renderItemIndex] = (RenderComponent){gRenderer, undefinedSheet, 0, destRect, 255, zPos};
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
		tileInSheet.x = (renderBuffer[i].tile % renderBuffer[i].tileSheet.w) * renderBuffer[i].tileSheet.tile_size;
		tileInSheet.y = (renderBuffer[i].tile / renderBuffer[i].tileSheet.w) * renderBuffer[i].tileSheet.tile_size;
		
		SDL_SetTextureAlphaMod(renderBuffer[i].tileSheet.tex, renderBuffer[i].alpha);
		SDL_Rect sourceRect = {tileInSheet.x, tileInSheet.y, renderBuffer[i].tileSheet.tile_size, renderBuffer[i].tileSheet.tile_size};
		SDL_RenderCopy(renderBuffer[i].renderer, renderBuffer[i].tileSheet.tex, &sourceRect, &renderBuffer[i].transform);	
	}
	SetupRenderFrame();
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
			// mapArray[y][x].block = malloc(sizeof(BlockComponent));
			mapArray[y][x].type = strtol(parsedInt, NULL, 10);
			// if(strtol(parsedInt, NULL, 10) > numBlocks || strtol(parsedInt, NULL, 10) == -1){
			// 	mapArray[y][x].block = &undefinedBlock;
			// }else{
			// 	mapArray[y][x].block = &blockData[strtol(parsedInt, NULL, 10)];
			// 	// printf("%d\n", mapArray[y][x].block->tile);
			// 	printf("%d\n", strtol(parsedInt, NULL, 10));
			// }
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
int mapSize = 32;
// void DrawMap(SDL_Texture *textureSheet, int sheetWidth, int mapArray[][32]){
void DrawMap(WB_Tilesheet tileSheet, RenderTileComponent mapArray[][32], int zPos){//Draw map from 2D array
	for(int y = (mapOffsetPos.y / tileSize - 1) * ((mapOffsetPos.y / tileSize - 1) > 0); y < ((mapOffsetPos.y + HEIGHT) / tileSize + 1) && y < mapSize; y++){
		for(int x = (mapOffsetPos.x / tileSize - 1) * ((mapOffsetPos.x / tileSize - 1) > 0); x < ((mapOffsetPos.x + WIDTH) / tileSize + 1) && x < mapSize; x++){
			if(mapArray[y][x].type != -1){
				Vector2 tilePos = {(x * tileStretchSize) - mapOffsetPos.x, (y * tileStretchSize) - mapOffsetPos.y};

				SDL_Rect tile = {tilePos.x, tilePos.y, tileStretchSize, tileStretchSize};
				AddToRenderQueue(gRenderer, tileSheet, mapArray[y][x].type, tile, -1, zPos + mapArray[y][x].zPos);
				// AddToRenderQueue(gRenderer, tileSheet, mapArray[y][x].block->tile, tile, -1, zPos + mapArray[y][x].zPos);
				mapArray[y][x].zPos = 0;
			}
		}
	}
}

void DrawLevel(){
	// DrawMap(*find_tilesheet("default_ground"), map, 0);
	DrawMap(*find_tilesheet("default_ground"), buildLayer, 0);

	DrawMap(*find_tilesheet("default_ground"), map1, 0);
	DrawMap(*find_tilesheet("default_ground"), customMap, 0);


	DrawMap(*find_tilesheet("furniture"), furnitureMap, 1);
	DrawCharacter(characterFacing, 6);
}

/* void TILE_SetTile(int mapArray[][32], int ) */