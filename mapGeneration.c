#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "headers/DataTypes.h"
#include "headers/ECS.h"
#include "headers/data.h"
#include "headers/tileMap.h"
#include "headers/mapGeneration.h"
#include "headers/inventory.h"

RenderTileComponent buildLayer_tmp[32][32] = {};
enum types {GRASS = 0, WATER = 47};
bool doRandomGen = true;

void CreateTempLayerMap(RenderTileComponent **layer, Vector2 size){
	layer = calloc(size.y, sizeof(RenderTileComponent));
	for(int y = 0; y < size.y; y++){
		layer[y] = calloc(size.x, sizeof(RenderTileComponent));
		memset(layer[y], 0, sizeof(layer) * size.x);
	}
	// printf("%d\n", sizeof(layer));
}

int GetSurroundCount(RenderTileComponent *layer, Vector2 tile){
	int surroundCount = 0;
	for(int y = tile.y - 1; y <= tile.y + 1; y++){
		for(int x = tile.x - 1; x <= tile.x + 1; x++){
			if(x >= 0 && x < level->map_size.x && y >= 0 && y < level->map_size.y){
				if(x != tile.x || y != tile.y){
					if(layer[y][x].block->tile < WATER){
						surroundCount++;
					}
				}
			}
		}
	}
	// printf("%d\n", surroundCount);
	return surroundCount;
}

void FillMap(RenderTileComponent map[][32]){
	for(int y = 0; y < 32; y++){
		for(int x = 0; x < 32; x++){
			map[y][x].block = find_block("grass");
		}
	}
}

void GenerateProceduralMap(int ratioPercent, int smoothSteps){
	// FillMap(levels[0].terrain);
	// RandomMap(levels[0].terrain, ratioPercent, 2, find_block("grass"), find_block("water"));
	// RandomMap(levels[0].terrain, ratioPercent, "grass", "water", NULL);
	// for(int i = 0; i < smoothSteps; i++){
		SmoothMap(&levels[0], find_block("grass"), find_block("water"));
	// }
	// PlaceBlock((Vector2){6, 6}, find_block("nylium"));
	// PlaceBlock((Vector2){7, 5}, find_block("nylium"));
	// PlaceBlock((Vector2){8, 5}, find_block("nylium"));
	// PlaceBlock((Vector2){4, 4}, find_block("nylium"));
	// PlaceBlock((Vector2){6, 4}, find_block("grass"));
	// DefineCollisions();
	// AutotileMap(levels[0].terrain, autotileData[0]);
	// AutotileMap(levels[0].terrain, autotileData[1]);
}

void AutotileMap(RenderTileComponent map[][32], AutotileComponent autotile){
	/* TURNED 90 DEGREES COUNTER-CLOCKWISE
	 1  |  2  | 4
	____|_____|____
	 8  | /// | 16
	____|_____|____
	32  | 64  |128
	*/
	const int correlationArray[46] = {2, 8, 10, 11, 16, 18, 22, 24, 26, 27, 30, 31, 64, 66, 72, 74, 75, 80, 82, 86, 88, 90, 91, 94, 95, 104, 106, 107, 120, 122, 123, 126, 127, 208, 210, 214, 216, 218, 219, 222, 223, 248, 250, 251, 254};
	for(int y = 0; y < 32; y++){
		for(int x = 0; x < 32; x++){
			if(strcmp(map[y][x].block->item->name, autotile.baseBlock->item->name) == 0 ||strcmp(map[y][x].block->item->name, autotile.subBlock->item->name) == 0){
			int adjacentCaseNumber = 0;			
			int cornerCaseNumber = 0;	
			int finalCaseNumber = 0;
			//Auto Tiling

			int yNeg = (y - 1) * (y != 0);
			int yPos = (y + 1) * (y != 31);
			int xNeg = (x - 1) * (x != 0);
			int xPos = (x + 1) * (x != 31);
			//Corner
			bool bottomRight = strcmp(map[yNeg][xPos].block->item->name, autotile.subBlock->item->name) != 0;
			bool bottomLeft = strcmp(map[yNeg][xNeg].block->item->name, autotile.subBlock->item->name) != 0;
			bool topRight = strcmp(map[yPos][xPos].block->item->name, autotile.subBlock->item->name) != 0;
			bool topLeft = strcmp(map[yPos][xNeg].block->item->name, autotile.subBlock->item->name) != 0;
			//Adjacent
			bool bottom = strcmp(map[yNeg][x].block->item->name, autotile.subBlock->item->name) != 0;
			bool right = strcmp(map[y][xPos].block->item->name, autotile.subBlock->item->name) != 0;
			bool left = strcmp(map[y][xNeg].block->item->name, autotile.subBlock->item->name) != 0;
			bool top = strcmp(map[yPos][x].block->item->name, autotile.subBlock->item->name) != 0;

			if(map[y][x].block->tile < WATER){
				//Reduce possible options down to 47
				if(topLeft){
					if(!top || !left){
						topLeft = false;
					}
				}
				if(topRight){
					if(!top || !right){
						topRight = false;
					}
				}
				if(bottomLeft){
					if(!bottom || !left){
						bottomLeft = false;
					}
				}
				if(bottomRight){
					if(!bottom || !right){
						bottomRight = false;
					}
				}
				//Shift side bits into final number
				cornerCaseNumber += topLeft << 0;//CRN
				adjacentCaseNumber += top << 1;//ADJ
				cornerCaseNumber += topRight << 2;//CRN
				adjacentCaseNumber += left << 3;//ADJ
				adjacentCaseNumber += right << 4;//ADJ
				cornerCaseNumber += bottomLeft << 5;//CRN
				adjacentCaseNumber += bottom << 6;//ADJ
				cornerCaseNumber += bottomRight << 7;//CRN
				finalCaseNumber = adjacentCaseNumber + cornerCaseNumber;

				//Filter the 47 possible options using the correlation array
				if(finalCaseNumber != 0 && finalCaseNumber != 255){
					for(int i = 0; i < 47; i++){
						if(finalCaseNumber == correlationArray[i - 1]){
							finalCaseNumber = i;
							break;
						}
					}
				}
				// if(finalCaseNumber == GRASS){
				// printf("%d\n", &blockData[8].id);
				// printf("%d\n", autotile.auto_block[3]->id);
				// 	levels[0].terrain[y][x].block = autotileData[0].baseBlock;
				// }else if(finalCaseNumber == WATER){
				// 	levels[0].terrain[y][x].block = autotileData[0].subBlock;
				// }
				if(finalCaseNumber != 0 && finalCaseNumber != 255){
					levels[0].terrain[y][x].block = &autotile.auto_block[finalCaseNumber - 1];
				}
			}

			if(levels[0].terrain[y][x].block->tile == WATER){
				levels[0].collision[y][x] = 0;
			}else if(levels[0].terrain[y][x].block->tile > GRASS){
				levels[0].collision[y][x] = -1;
			}

			}
		}
	}
}

void SmoothMap(LevelComponent *level, BlockComponent *main, BlockComponent *secondary){
	RenderTileComponent **tempLayer = {0};
	CreateTempLayerMap(tempLayer, (Vector2){level->map_size.x, level->map_size.y});
	// LevelComponent tempLevel;
	// CreateLevel(tempLevel, (Vector2){level->map_size.x, level->map_size.y});
	for(int y = 0; y < level->map_size.y; y++){
		for(int x = 0; x < level->map_size.x; x++){
			if(level->terrain[y][x].block->tile < WATER){
				// buildLayer_tmp[y][x].block = find_block("grass");
				// tempLayer[y][x].block = find_block("grass");
				buildLayer_tmp[y][x].block = main;
			}else{
				// buildLayer_tmp[y][x].block = find_block("water");
				// tempLayer[y][x].block = find_block("water");
				buildLayer_tmp[y][x].block = secondary;
			}
		}
	}
	for(int y = 0; y < level->map_size.y; y++){
		for(int x = 0; x < level->map_size.x; x++){
			int surroundTiles = GetSurroundCount(level, (Vector2){x, y});
	printf ("here\n");
			if(surroundTiles > 4){
				level->terrain[y][x].block = main;
				// buildLayer_tmp[y][x].block = find_block("grass");
				// tempLayer[y][x].block = find_block("grass");
				tempLayer[y][x].block = main;
				// level->terrain[y][x].block = main;
				// buildLayer_tmp[y][x].block = main;
			}else if(surroundTiles < 4){
				level->terrain[y][x].block = secondary;
				// buildLayer_tmp[y][x].block = find_block("water");
				// tempLayer[y][x].block = find_block("water");
				tempLayer[y][x].block = secondary;
				// level->terrain[y][x].block = secondary;
				// buildLayer_tmp[y][x].block = secondary;
			}
		}
	}
	free(tempLayer);
}
void RandomMap(RenderTileComponent map[][32], int ratioPercent, int numTypes, ...){
	time_t rawTime;
	struct tm *timeinfo;
	time(&rawTime);
	timeinfo = localtime(&rawTime);
	int worldSeed = timeinfo->tm_sec;
	// SeedLehmer(worldSeed, 0, 0);
	SeedLehmer(28, 0, 0);

	va_list valist;
	va_start(valist, numTypes);
	// va_start(valist, str);

	BlockComponent *blocks;
	blocks = malloc(sizeof(BlockComponent));
	// int i = 0;
	for(int i = 0; i < numTypes; i++){
	// while(str != NULL){
		blocks = realloc(blocks, (i + 1) * sizeof(BlockComponent));
		// char *tmp;
		// strcpy(tmp, va_arg(valist, const char *));

		// blocks[i] = realloc(blocks[i], strlen(str) * sizeof(char));

		// strcpy(blocks[i], str);

		// blocks[i] = str;
		// strcpy(str, va_arg(valist, const char *));
		// str = va_arg(valist, const char *);
		blocks[i] = va_arg(valist, BlockComponent);
		// i++;
	}
	va_end(valist);

	for(int y = 0; y < 32; y++){
		for(int x = 0; x < 32; x++){
			if(getRnd(0, 100) <= ratioPercent){
				// map[y][x].block = find_block("grass");
				// map[y][x].block = find_block(blocks[0]);
				map[y][x].block = &blocks[0];
			}else{
				// map[y][x].block = find_block(blocks[1]);
				map[y][x].block = &blocks[1];
			}
		}
	}
}

void DefineCollisions(){
	memset(levels[0].collision, -1, sizeof(levels[0].collision));
	for(int y = 1; y < 31; y++){
		for(int x = 1; x < 31; x++){
			//Efficient (Border around non collidable blocks) (31*31)
			if(levels[0].features[y][x].block->collisionType != -1){
				levels[0].collision[y][x] = levels[0].features[y][x].block->collisionType;
			}
			if(levels[0].terrain[y][x].block->collisionType == 0){
				bool up = (levels[0].terrain[y - 1][x].block->collisionType == -1);
				bool down = (levels[0].terrain[y + 1][x].block->collisionType == -1);
				bool left = (levels[0].terrain[y][x - 1].block->collisionType == -1);
				bool right = (levels[0].terrain[y][x + 1].block->collisionType == -1);

				if(up || down || left || right){
					levels[0].collision[y][x] = 0;
				}
			}
			/*//Simpler (Every collidable block has a collider) (32*32)
			if(levels[0].features[y][x].block->collisionType != -1){
					levels[0].collision[y][x] = levels[0].features[y][x].block->collisionType;
				}
			if(levels[0].terrain[y][x].block->collisionType != -1){
				levels[0].collision[y][x] = levels[0].terrain[y][x].block->collisionType;
			}*/
		}
	}
}

void TileMapEdit(RenderTileComponent tileMap[][32], Vector2 pos, BlockComponent *block, bool collide){
	
	tileMap[pos.y][pos.x].block = block;
	
	AutotileMap(levels[0].terrain, autotileData[0]);
}

void PlaceBlock(Vector2 tile, BlockComponent *block){
	//If the player is where the block is to be placed, only place it if its non collidable
	if((!SDL_HasIntersection(&character.collider.boundingBox, &(SDL_Rect){tile.x, tile.y, 64, 64}) || block->collisionType != 0)){
		if(strcmp(block->layer, "terrain") == 0){
			levels[0].terrain[tile.y][tile.x].block = block;
		}else if(strcmp(block->layer, "feature") == 0){
			levels[0].features[tile.y][tile.x].block = block;
		}
		DefineCollisions(levels[0].terrain);
	}
}