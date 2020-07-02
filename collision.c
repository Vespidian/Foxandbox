#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "headers/DataTypes.h"
#include "headers/ECS.h"

#include "headers/initialize.h"
#include "headers/data.h"
#include "headers/tileMap.h"
#include "headers/collision.h"


SDL_Rect charRect;
SDL_Rect charCollider;

Vector2 charTilePos = {0, 0};

Entity character;

void CheckCollisions(SDL_Rect tileR, CollisionComponent *col){
	SDL_Rect collider = col->collisionBox;
	SDL_Rect rectTop = (SDL_Rect){collider.x, collider.y - 1, collider.w, 1};
	SDL_Rect rectBottom = (SDL_Rect){collider.x, collider.y + collider.h, collider.w, 1};
	SDL_Rect rectLeft = (SDL_Rect){collider.x - 1, collider.y, 1, collider.h};
	SDL_Rect rectRight = (SDL_Rect){collider.x + collider.w, collider.y, 1, collider.h};
	col->colUp = false;
	col->colDown = false;
	col->colLeft = false;
	col->colRight = false;
	
	if(!col->noClip){
		if(SDL_HasIntersection(&tileR, &col->collisionBox)){
			Vector2 roundSpeed = {mapOffsetPos.x % 4, mapOffsetPos.y % 4};//Make sure the character position is always a multiple of 4\
			keeping everything pixel perfect
			if(roundSpeed.x != 0){
				mapOffsetPos.x -= roundSpeed.x;
			}
			if(roundSpeed.y != 0){
				mapOffsetPos.y -= roundSpeed.y;
			}
		}
		
		if(SDL_HasIntersection(&tileR, &rectTop)){
			col->colUp = true;
		}
		if(SDL_HasIntersection(&tileR, &rectBottom)){
			col->colDown = true;
		}
		if(SDL_HasIntersection(&tileR, &rectLeft)){
			col->colLeft = true;
		}
		if(SDL_HasIntersection(&tileR, &rectRight)){
			col->colRight = true;
		}
	}
}

void SetTileZPos(Vector2 tile, SDL_Rect tileR, SDL_Rect colRectBottom){
	if(tileR.y + tileSize < colRectBottom.y){
		furnitureMap[tile.y][tile.x].zPos = 1;
	}else{
		furnitureMap[tile.y][tile.x].zPos = RNDRLYR_PLAYER + 1;
	}
}

/* int EntityCollision(CollisionComponent *col, Vector2 tilePosition){
	SDL_Rect collider = col->collisionBox;
	SDL_Rect rectTop = (SDL_Rect){collider.x, collider.y - 1, collider.w, 1};
	SDL_Rect rectBottom = (SDL_Rect){collider.x, collider.y + collider.h, collider.w, 1};
	SDL_Rect rectLeft = (SDL_Rect){collider.x - 1, collider.y, 1, collider.h};
	SDL_Rect rectRight = (SDL_Rect){collider.x + collider.w, collider.y, 1, collider.h};
	
	if(enableHitboxes){
		AddToRenderQueue(gRenderer, debugSheet, 4, rectTop, -1, 1001);
		AddToRenderQueue(gRenderer, debugSheet, 4, rectBottom, -1, 1001);
		AddToRenderQueue(gRenderer, debugSheet, 4, rectLeft, -1, 1001);
		AddToRenderQueue(gRenderer, debugSheet, 4, rectRight, -1, 1001);
		
		AddToRenderQueue(gRenderer, debugSheet, 5, col->collisionBox, -1, 1000);
	}
	
	for(int y = tilePosition.y - 1; y <= tilePosition.y + 1; y++){
		for(int x = tilePosition.x - 1; x <= tilePosition.x + 1; x++){
			SDL_Rect tileR = {(x * tileSize) - mapOffsetPos.x, (y * tileSize) - mapOffsetPos.y, tileSize, tileSize};
			if(SDL_HasIntersection(&tileR, &col->boundingBox) && colMap[y][x] != -1){
				
				if(colMap[y][x] == 0){
					CheckCollisions(tileR, col);
					
				}else if(colMap[y][x] == 1){
					SetTileZPos((Vector2){x, y}, tileR, rectBottom);
					
				}else if(colMap[y][x] == 2){
					furnitureMap[y][x].zPos = RNDRLYR_PLAYER + 1;
					
				}else if(colMap[y][x] == 8){//HALF YELLOW
					SetTileZPos((Vector2){x, y}, tileR, rectBottom);
					tileR.y = (y * tileSize) - mapOffsetPos.y + tileSize / 2;
					tileR.h = tileSize / 2;
					CheckCollisions(tileR, &character.collider);
					
				}
				
				if(enableHitboxes){
					AddToRenderQueue(gRenderer, debugSheet, colMap[y][x], tileR, -1, 1000);
				}
			}
		}
	}
}  */
int EntityCollision(Entity *ent){
	Vector2 tilePosition = ent->transform.tilePos;
	SDL_Rect collider = ent->collider.collisionBox;
	
	SDL_Rect rectTop = (SDL_Rect){collider.x, collider.y - 1, collider.w, 1};
	SDL_Rect rectBottom = (SDL_Rect){collider.x, collider.y + collider.h, collider.w, 1};
	SDL_Rect rectLeft = (SDL_Rect){collider.x - 1, collider.y, 1, collider.h};
	SDL_Rect rectRight = (SDL_Rect){collider.x + collider.w, collider.y, 1, collider.h};
	
	if(enableHitboxes){
		AddToRenderQueue(gRenderer, debugSheet, 4, rectTop, -1, 1001);
		AddToRenderQueue(gRenderer, debugSheet, 4, rectBottom, -1, 1001);
		AddToRenderQueue(gRenderer, debugSheet, 4, rectLeft, -1, 1001);
		AddToRenderQueue(gRenderer, debugSheet, 4, rectRight, -1, 1001);
		
		AddToRenderQueue(gRenderer, debugSheet, 5, collider, -1, 1000);
		AddToRenderQueue(gRenderer, debugSheet, 3, ent->collider.boundingBox, -1, 1000);
	}
	
	for(int y = tilePosition.y - 1; y <= tilePosition.y + 1; y++){
		for(int x = tilePosition.x - 1; x <= tilePosition.x + 1; x++){
			SDL_Rect tileR = {(x * tileSize) - mapOffsetPos.x, (y * tileSize) - mapOffsetPos.y, tileSize, tileSize};
			if(SDL_HasIntersection(&tileR, &ent->collider.boundingBox) && colMap[y][x] != -1){
				
				if(colMap[y][x] == 0){
					CheckCollisions(tileR, &ent->collider);
					
				}else if(colMap[y][x] == 1){
					SetTileZPos((Vector2){x, y}, tileR, rectBottom);
					
				}else if(colMap[y][x] == 2){
					furnitureMap[y][x].zPos = RNDRLYR_PLAYER + 1;
					
				}else if(colMap[y][x] == 8){//HALF YELLOW
					SetTileZPos((Vector2){x, y}, tileR, rectBottom);
					tileR.y = (y * tileSize) - mapOffsetPos.y + tileSize / 2;
					tileR.h = tileSize / 2;
					CheckCollisions(tileR, &ent->collider);
					
				}
				
				if(enableHitboxes){
					AddToRenderQueue(gRenderer, debugSheet, colMap[y][x], tileR, -1, 1000);
				}
			}
		}
	}
}

void FindCollisions(){
	charRect = (SDL_Rect){(WIDTH / 2 - tileSize / 2), (HEIGHT / 2 - tileSize / 2), tileSize, tileSize + 2};
	
	SDL_Rect colBox = {charRect.x + 12, charRect.y + tileSize - 4, tileSize - 24, 4};
	character.collider.collisionBox = colBox;
	character.collider.boundingBox = charRect;
	// playerCoord
	EntityCollision(&character);
	// EntityCollision(&character.collider, character.transform.tilePos);
	// EntityCollision(&character.collider, playerCoord);
	// character.collider.colLeft = true;	
	
	

	
	/*
	// for(int y = 0; y < 32; y++){
		// for(int x = 0; x < 32; x++){
	for(int y = playerCoord.y - 1; y < playerCoord.y + 1; y++){
		for(int x = playerCoord.x - 1; x < playerCoord.x + 1; x++){
			SDL_Rect tileR = {(x * tileSize) - mapOffsetPos.x, (y * tileSize) - mapOffsetPos.y, tileSize, tileSize};
			
			// if(SDL_PointInRect(&charP, &tileR)){
				// printf("%d, %d\n", x, y);
				// charTilePos = (Vector2){x, y};
			// }
			
			if(SDL_HasIntersection(&tileR, &charRect) && colMap[y][x] != -1){
				if(colMap[y][x] == 0){//YELLOW
					furnitureMap[y][x].zPos = 1;
					CheckCollisions(tileR, character.collider);
					
				}else if(colMap[y][x] == 1){//BLUE
					SetTileZPos((Vector2){x, y}, tileR, charCollider_bottom);
				
				}else if(colMap[y][x] == 2){//RED
					//Only put the character behind if the character is intersecting with said tile
					furnitureMap[y][x].zPos = RNDRLYR_PLAYER + 1;
					
				}else if(colMap[y][x] == 8){//HALF YELLOW
					SetTileZPos((Vector2){x, y}, tileR, charCollider_bottom);
					tileR.y = (y * tileSize) - mapOffsetPos.y + tileSize / 2;
					tileR.h = tileSize / 2;
					CheckCollisions(tileR, character.collider);
					
				}else if(colMap[y][x] == 16){//
					tileR.h = tileSize / 2;
					CheckCollisions(tileR, character.collider);
					
				}else if(colMap[y][x] == 9){//
					tileR.w = tileSize / 2;
					CheckCollisions(tileR, character.collider);
					
				}else if(colMap[y][x] == 17){//
					tileR.x = x * tileSize - mapOffsetPos.x + tileSize / 2;
					tileR.w = tileSize / 2;
					CheckCollisions(tileR, character.collider);
					
				}
				
				if(colMap[y][x] != -1 && enableHitboxes){
					SDL_RenderDrawRect(gRenderer, &tileR);
					AddToRenderQueue(gRenderer, debugSheet, colMap[y][x], tileR, -1, 1000);
				}
			}
		}
	}*/
	
	/*Vector2 entityPos;
	for(int y = entityPos.y - 1; y < entityPos.y + 1; y++){
		for(int x = entityPos.x - 1; x < entityPos.y + 1; x++){
			// Vector2 playerPos = {0, 0};
			SDL_Rect tileR = {(x * tileSize) - mapOffsetPos.x, (y * tileSize) - mapOffsetPos.y, tileSize, tileSize};
			
		}
	}*/
}
