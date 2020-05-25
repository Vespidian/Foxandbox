#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include "headers/DataTypes.h"
#include "headers/ECS.h"

#include "headers/initialize.h"
#include "headers/data.h"
#include "headers/drawFunctions.h"
#include "headers/tileMap.h"
#include "headers/collision.h"

bool noClip = false;

SDL_Rect charCollider_bottom;
SDL_Rect charCollider_right;
SDL_Rect charCollider_left;
SDL_Rect charCollider_top;

bool colUp;
bool colDown;
bool colLeft;
bool colRight;

SDL_Rect charRect;
SDL_Rect charCollider;

int currentCollisions[3] = {-1, -1, -1};

Vector2 charTilePos = {0, 0};

void CheckCollisions(SDL_Rect tileR){
	if(!noClip){
		if(SDL_HasIntersection(&tileR, &charCollider_top)){
			colUp = true;
		}
		if(SDL_HasIntersection(&tileR, &charCollider_bottom)){
			colDown = true;
		}
		if(SDL_HasIntersection(&tileR, &charCollider_left)){
			colLeft = true;
		}
		if(SDL_HasIntersection(&tileR, &charCollider_right)){
			colRight = true;
		}
	}
}

void SetTileZPos(Vector2 tile, SDL_Rect tileR, SDL_Rect charRect){
	if(tileR.y + tileSize < charCollider_bottom.y){
		passableMap[tile.y][tile.x].zPos = 1;
		furnitureMap[tile.y][tile.x].zPos = 1;
	}else{
		passableMap[tile.y][tile.x].zPos = RNDRLYR_PLAYER + 1;
		furnitureMap[tile.y][tile.x].zPos = RNDRLYR_PLAYER + 1;
	}
}

void FindCollisions(){
	colUp = false;
	colDown = false;
	colLeft = false;
	colRight = false;
	
	charRect = (SDL_Rect){(WIDTH / 2 - tileSize / 2), (HEIGHT / 2 - tileSize / 2), tileSize, tileSize + 2};
	// AddToRenderQueue(gRenderer, debugSheet, 0, charRect, 1000);//Debug PlayerBox

	charCollider_bottom = (SDL_Rect){charRect.x + 4, charRect.y + tileSize, tileSize - 8, 1};
	charCollider_right = (SDL_Rect){charRect.x + tileSize - 4, charRect.y + 60, 1, 4};
	charCollider_left = (SDL_Rect){charRect.x + 3, charRect.y + tileSize - 4, 1, 4};
	charCollider_top = (SDL_Rect){charRect.x + 4, charRect.y + 59, tileSize - 8, 1};
	
	SDL_Point charP = {charRect.x + tileSize / 2, charRect.y + tileSize / 2};
	
	for(int y = 0; y < 32; y++){
		for(int x = 0; x < 32; x++){
			SDL_Rect tileR = {(x * tileSize) - mapOffsetPos.x, (y * tileSize) - mapOffsetPos.y, tileSize, tileSize};
			
			// if(SDL_PointInRect(&charP, &tileR)){
				// printf("%d, %d\n", x, y);
				// charTilePos = (Vector2){x, y};
			// }
			
			if(SDL_HasIntersection(&tileR, &charRect) && colMap[y][x] != -1){
				if(colMap[y][x] == 0){//YELLOW
					passableMap[y][x].zPos = 1;
					furnitureMap[y][x].zPos = 1;
					CheckCollisions(tileR);
					
				}else if(colMap[y][x] == 1){//BLUE
					SetTileZPos((Vector2){x, y}, tileR, charCollider_bottom);
				
				}else if(colMap[y][x] == 2){//RED
					//Only put the character behind if the character is intersecting with said tile
					passableMap[y][x].zPos = RNDRLYR_PLAYER + 1;
					furnitureMap[y][x].zPos = RNDRLYR_PLAYER + 1;
					
				}else if(colMap[y][x] == 8){//HALF YELLOW
					SetTileZPos((Vector2){x, y}, tileR, charCollider_bottom);
					tileR.y = (y * tileSize) - mapOffsetPos.y + tileSize / 2;
					tileR.h = tileSize / 2;
					CheckCollisions(tileR);
					
				}else if(colMap[y][x] == 16){//
					tileR.h = tileSize / 2;
					CheckCollisions(tileR);
					
				}else if(colMap[y][x] == 9){//
					tileR.w = tileSize / 2;
					CheckCollisions(tileR);
					
				}else if(colMap[y][x] == 17){//
					tileR.x = x * tileSize - mapOffsetPos.x + tileSize / 2;
					tileR.w = tileSize / 2;
					CheckCollisions(tileR);
					
				}
				
				if(colMap[y][x] != -1 && enableHitboxes){
					SDL_RenderDrawRect(gRenderer, &tileR);
					AddToRenderQueue(gRenderer, debugSheet, colMap[y][x], tileR, 1000);
				}
			}
		}
	}
	/*Vector2 entityPos;
	for(int y = entityPos.y - 1; y < entityPos.y + 1; y++){
		for(int x = entityPos.x - 1; x < entityPos.y + 1; x++){
			// Vector2 playerPos = {0, 0};
			SDL_Rect tileR = {(x * tileSize) - mapOffsetPos.x, (y * tileSize) - mapOffsetPos.y, tileSize, tileSize};
			
		}
	}*/
}