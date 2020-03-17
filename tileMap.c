#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include "headers/DataTypes.h"
#include "headers/initialize.h"
#include "headers/data.h"
#include "headers/drawFunctions.h"
#include "headers/tileMap.h"

int tilePixelSize = 16;
int tileStretchSize = 64;
int tileSheetWidth = 8;


int colMap[32][32] = {};
int map[32][32] = {};
int furnitureMap[32][32] = {};
int passableMap[32][32] = {};

// int passableMap[32][32] = {};

void LoadImage(SDL_Rect destRect, SDL_Rect sourceRect, SDL_Texture *sourceTexture) {
	SDL_RenderCopy(gRenderer, sourceTexture, &sourceRect, &destRect);
}

Vector2 GetTileSheetLocation(int tileNum){
	for(int y = 0; y < tileSheetWidth; y++){
		for(int x = 0; x < tileSheetWidth; x++){
			if(x + y * (tileSheetWidth) == tileNum){
				Vector2 tileInSheet = {x * tilePixelSize, y * tilePixelSize};
				return tileInSheet;
			}
		}
	}
}

void LoadMap(char *fileLoc, int mapArray[][32]){
	FILE *fp = fopen(fileLoc, "r");
	char buffer[256];
	char *parsedInt;
	
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
}

void DrawMap(SDL_Texture *textureSheet, int mapArray[][32]){
	for(int y = 0; y < 32; y++){
		for(int x = 0; x < 32; x++){
			if(mapArray[y][x] != -1){
				Vector2 tilePos = {(x * tileStretchSize) + worldPosition.x, (y * tileStretchSize) + worldPosition.y};
				
				SDL_Point p = {tilePos.x, tilePos.y};
				SDL_Rect r = {-tileStretchSize, -tileStretchSize, WIDTH + tileStretchSize, HEIGHT + tileStretchSize};
				
				//Check if tile is in viewport and only render it if it is
				if(SDL_PointInRect(&p, &r)){
					SDL_Rect tile = {tilePos.x, tilePos.y, tileStretchSize, tileStretchSize};
					for(int i = 0; i < tileSheetWidth * tileSheetWidth; i++){
						if(mapArray[y][x] == i){
							Vector2 tilePosInSheet = GetTileSheetLocation(i);
							SDL_Rect tileLocation = {tilePosInSheet.x, tilePosInSheet.y, 16, 16};
							LoadImage(tile, tileLocation, textureSheet);
						}
					}
					// Highlight the tile the mouse is currently on
					// if(map[y][x] != 3){
						SDL_GetMouseState(&mousePos.x, &mousePos.y);
						SDL_Point mousePoint = {mousePos.x, mousePos.y};
						if(SDL_PointInRect(&mousePoint, &tile)){
							SDL_Rect mouseHighlight = {(x * tileStretchSize) + worldPosition.x, (y * tileStretchSize) + worldPosition.y, tileStretchSize, tileStretchSize};
							SDL_SetRenderDrawColor(gRenderer, 255, 211, 0, 0xff);
							SDL_RenderDrawRect(gRenderer, &mouseHighlight);
							// printf("%d\n", mapArray[y][x]);
						}
					// }
				}
			}
		}
	}
}