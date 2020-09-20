#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

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
	
	if(!col->noClip){
		
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
	}
}

void SetTileZPos(Vector2 tile, SDL_Rect tileR, SDL_Rect colRectBottom){
	if(tileR.y + tileSize < colRectBottom.y){
		activeLevel->features[tile.y][tile.x].zPos = 1;
	}else{
		activeLevel->features[tile.y][tile.x].zPos = RNDRLYR_PLAYER + 1;
	}
}

int EntityCollision(Entity *ent){
	Vector2 tilePosition = ent->transform.tilePos;
	SDL_Rect collider = ent->collider.collisionBox;
	
	SDL_Rect rectTop = (SDL_Rect){collider.x, collider.y - 1, collider.w, 1};
	SDL_Rect rectBottom = (SDL_Rect){collider.x, collider.y + collider.h, collider.w, 1};
	SDL_Rect rectLeft = (SDL_Rect){collider.x - 1, collider.y, 1, collider.h};
	SDL_Rect rectRight = (SDL_Rect){collider.x + collider.w, collider.y, 1, collider.h};
	
	if(enableHitboxes){
		AddToRenderQueue(renderer, &debugSheet, 4, rectTop, -1, 1001);
		AddToRenderQueue(renderer, &debugSheet, 4, rectBottom, -1, 1001);
		AddToRenderQueue(renderer, &debugSheet, 4, rectLeft, -1, 1001);
		AddToRenderQueue(renderer, &debugSheet, 4, rectRight, -1, 1001);
		
		AddToRenderQueue(renderer, &debugSheet, 5, collider, -1, 1000);
		AddToRenderQueue(renderer, &debugSheet, 3, ent->collider.boundingBox, -1, 1000);
	}
	
	ent->collider.colUp = false;
	ent->collider.colDown = false;
	ent->collider.colLeft = false;
	ent->collider.colRight = false;
	
	if(tilePosition.y >= 0 && tilePosition.y < activeLevel->map_size.y - 1 && tilePosition.x >= 0 && tilePosition.x < activeLevel->map_size.x - 1){
		SDL_Rect tileR = {tileSize - mapOffsetPos.x, tileSize - mapOffsetPos.y, tileSize, tileSize};
		for(int y = tilePosition.y - (tilePosition.y > 0 ? 1 : 0); y <= tilePosition.y + (tilePosition.y != activeLevel->map_size.y ? 1 : 0); y++){
			for(int x = tilePosition.x - 1; x <= tilePosition.x + 1; x++){
				tileR = (SDL_Rect){(x * tileSize) - mapOffsetPos.x, (y * tileSize) - mapOffsetPos.y, tileSize, tileSize};
				if(SDL_HasIntersection(&tileR, &ent->collider.boundingBox) && activeLevel->collision[y][x] != -1){
					if(enableHitboxes){
						AddToRenderQueue(renderer, &debugSheet, activeLevel->collision[y][x], tileR, -1, 1000);
					}
					switch(activeLevel->collision[y][x]){
						case 0:
							CheckCollisions(tileR, &ent->collider);
							break;
						case 1:
							SetTileZPos((Vector2){x, y}, tileR, rectBottom);
							break;
						case 2:
							activeLevel->features[y][x].zPos = RNDRLYR_PLAYER + 1;
							break;
						case 8:
							SetTileZPos((Vector2){x, y}, tileR, rectBottom);
							tileR.y = (y * tileSize) - mapOffsetPos.y + tileSize / 2;
							tileR.h = tileSize / 2;
							CheckCollisions(tileR, &ent->collider);							
							break;
						default:
							break;
					}
				}
			}
		}
	}

	if(SDL_HasIntersection(&ent->collider.collisionBox, &mapRect)){
		if(ent->collider.collisionBox.y - 4 <= mapRect.y){
			ent->collider.colUp = true;
		}
		if((ent->collider.collisionBox.y + 4 + ent->collider.collisionBox.h) >= mapRect.y + mapRect.h){
			ent->collider.colDown = true;
		}

		if(ent->collider.collisionBox.x - 4 <= mapRect.x){
			ent->collider.colLeft = true;
		}
		if((ent->collider.collisionBox.x + 4 + ent->collider.collisionBox.w) >= mapRect.x + mapRect.w){
			ent->collider.colRight = true;
		}
	}
}

void FindCollisions(){
	charRect = (SDL_Rect){(WIDTH / 2 - tileSize / 2), (HEIGHT / 2 - tileSize / 2), tileSize, tileSize + 2};
	SDL_Rect colBox = {charRect.x + 12, charRect.y + tileSize - 4, tileSize - 24, 4};
	
	character.collider.collisionBox = colBox;
	character.collider.boundingBox = charRect;

	EntityCollision(&character);
}
