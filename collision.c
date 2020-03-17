#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include "headers/DataTypes.h"
#include "headers/initialize.h"
#include "headers/data.h"
#include "headers/drawFunctions.h"
#include "headers/tileMap.h"
#include "headers/collision.h"

SDL_Rect charCollider_bottom;
SDL_Rect charCollider_right;
SDL_Rect charCollider_left;
SDL_Rect charCollider_top;

bool colUp;
bool colDown;
bool colLeft;
bool colRight;

SDL_Rect charCollider;

SDL_Rect setRect(int x, int y, int w, int h){
	SDL_Rect returnRect = {x, y, w, h};
	return returnRect;
}

void CheckCollisions(SDL_Rect tileR){
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

void ChangeOrder(SDL_Rect tileR, SDL_Rect charCollider, int def, int changed){
	// printf("%d", layerOrder);
	if(tileR.y + tileSize < charCollider_bottom.y){
		// isBehind = true;
		layerOrder = def;
	}else{
		layerOrder = changed;
		// isBehind = false;
	}
}

void FindCollisions(){
	colUp = false;
	colDown = false;
	colLeft = false;
	colRight = false;
	
	SDL_Rect charCollider = {(WIDTH / 2 - tileSize / 2), (HEIGHT / 2 - tileSize / 2), tileSize, tileSize + 1};
	/* DEFAULT SQUARE PLAYER COLLIDER
	SDL_Rect charCollider_bottom = {(WIDTH / 2 - tileSize / 2), (HEIGHT / 2 - tileSize / 2) + tileSize, tileSize, 1};
	SDL_Rect charCollider_right = {(WIDTH / 2 - tileSize / 2) + tileSize, (HEIGHT / 2 - tileSize / 2), 1, tileSize};
	SDL_Rect charCollider_left = {(WIDTH / 2 - tileSize / 2) - 1, (HEIGHT / 2 - tileSize / 2), 1, tileSize};
	SDL_Rect charCollider_top = {(WIDTH / 2 - tileSize / 2), (HEIGHT / 2 - tileSize / 2) - 1, tileSize, 1};
	*/
	
	charCollider_bottom = setRect((WIDTH / 2 - tileSize / 2) + 4, (HEIGHT / 2 - tileSize / 2) + tileSize, tileSize - 8, 1);
	charCollider_right = setRect((WIDTH / 2 - tileSize / 2) + tileSize - 4, (HEIGHT / 2 - tileSize / 2) + 60, 1, 4);
	charCollider_left = setRect((WIDTH / 2 - tileSize / 2) + 3, (HEIGHT / 2 - tileSize / 2) + 60, 1, 4);
	charCollider_top = setRect((WIDTH / 2 - tileSize / 2) + 4, (HEIGHT / 2 - tileSize / 2) + 59, tileSize - 8, 1);
	
	for(int y = 0; y < 32; y++){
		for(int x = 0; x < 32; x++){
				// printf("t");
			SDL_Rect tileR = {(x * tileSize) + worldPosition.x, (y * tileSize) + worldPosition.y, tileSize, tileSize};
			if(SDL_HasIntersection(&tileR, &charCollider) && colMap[y][x] != -1){
				if(colMap[y][x] == 0){//YELLOW
					SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, 0);
					CheckCollisions(tileR);
					
				}else if(colMap[y][x] == 1){//BLUE
					SDL_SetRenderDrawColor(gRenderer, 0, 0, 150, 0);
					//Only put the character behind if the character is intersecting with said tile
					ChangeOrder(tileR, charCollider, 0, 1);
					
				}else if(colMap[y][x] == 8){//YELLOW
					SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, 0);
					ChangeOrder(tileR, charCollider, 0, 2);
					
					tileR.y = (y * tileSize) + worldPosition.y + tileSize / 2;
					tileR.h = tileSize / 2;
					CheckCollisions(tileR);
					
				}else if(colMap[y][x] == 16){//
					SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, 0);
					ChangeOrder(tileR, charCollider, 0, 2);
					
					tileR.h = tileSize / 2;
					CheckCollisions(tileR);
					
				}else if(colMap[y][x] == 9){//
					SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, 0);
					ChangeOrder(tileR, charCollider, 0, 2);
					
					tileR.w = tileSize / 2;
					CheckCollisions(tileR);
					
				}else if(colMap[y][x] == 17){//
					SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, 0);
					ChangeOrder(tileR, charCollider, 0, 2);
					
					tileR.x = x * tileSize + worldPosition.x + tileSize / 2;
					tileR.w = tileSize / 2;
					CheckCollisions(tileR);
					
				}else if(colMap[y][x] == 24){//
					SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, 0);
					ChangeOrder(tileR, charCollider, 0, 2);
					
					tileR.x = x * tileSize + worldPosition.x + tileSize / 2;
					tileR.y = y * tileSize + worldPosition.y + tileSize / 2;
					tileR.w = tileSize / 2;
					tileR.h = tileSize / 2;
					CheckCollisions(tileR);
					
				}else if(colMap[y][x] == 25){//
					SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, 0);
					ChangeOrder(tileR, charCollider, 0, 2);
					
					tileR.y = y * tileSize + worldPosition.y + tileSize / 2;
					tileR.w = tileSize / 2;
					tileR.h = tileSize / 2;
					CheckCollisions(tileR);
					
				}else if(colMap[y][x] == 32){//
					SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, 0);
					ChangeOrder(tileR, charCollider, 0, 2);
					
					tileR.x = x * tileSize + worldPosition.x + tileSize / 2;
					tileR.w = tileSize / 2;
					tileR.h = tileSize / 2;
					CheckCollisions(tileR);
					
				}else if(colMap[y][x] == 33){//
					SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, 0);
					ChangeOrder(tileR, charCollider, 0, 2);
					
					tileR.w = tileSize / 2;
					tileR.h = tileSize / 2;
					CheckCollisions(tileR);
					
				}
				
				
				if(colMap[y][x] != -1 && enableHitboxes){
					SDL_RenderDrawRect(gRenderer, &tileR);
				}
			}
		}
	}
}