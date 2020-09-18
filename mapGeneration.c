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

void FillMap(LevelComponent *level, char *layer, BlockComponent *block){
	RenderTileComponent **specifiedLayer;
	if(strcmp(layer, "terrain") == 0){
		specifiedLayer = level->terrain;
	}else if(strcmp(layer, "features") == 0){
		specifiedLayer = level->features;
	}
	for(int y = 0; y < level->map_size.y; y++){
		for(int x = 0; x < level->map_size.x; x++){
			specifiedLayer[y][x].block = block;
		}
	}
}
void InitializeBlankLevel(LevelComponent *level, Vector2 size){
	if(size.x > MAXLEVELSIZE){
		size.x = MAXLEVELSIZE;
	}
	if(size.y > MAXLEVELSIZE){
		size.y = MAXLEVELSIZE;
	}
	level->map_size = (Vector2)size;

	level->terrain = malloc((size.y + 1) * sizeof(RenderTileComponent));
	level->features = malloc((size.y + 1) * sizeof(RenderTileComponent));
	level->collision = malloc((size.y + 1) * sizeof(uint64_t));
	for(int y = 0; y < size.y; y++){
		level->terrain[y] = malloc((size.x + 1) * sizeof(RenderTileComponent));
		level->features[y] = malloc((size.x + 1) * sizeof(RenderTileComponent));

		for(int x = 0; x < size.x; x++){
			level->terrain[y][x].block = find_block("air");
			level->features[y][x].block = find_block("air");
		}

		level->collision[y] = malloc((size.x + 1) * sizeof(int));
		memset(level->collision[y], -1, size.x * sizeof(int));
	}
}

int GetSurroundCount(LevelComponent *level, Vector2 tile, BlockComponent *type){
	int surroundCount = 0;
	for(int y = tile.y - 1; y <= tile.y + 1; y++){
		for(int x = tile.x - 1; x <= tile.x + 1; x++){
			if(x >= 0 && x < level->map_size.x && y >= 0 && y < level->map_size.y){
				if(x != tile.x || y != tile.y){
					if(strcmp(level->terrain[y][x].block->item->name, type->item->name) == 0){
						surroundCount++;
					}
				}
			}
		}
	}
	return surroundCount;
}

void GenerateFlowers(LevelComponent *level, int ratio){
	for(int y = 0; y < level->map_size.y; y++){
		for(int x = 0; x < level->map_size.x; x++){
			if(getRnd(0, 100) <= ratio && level->terrain[y][x].block->collisionType != 0){
				level->features[y][x].block = find_block("flower");
				// printf("flower\n");
			}
		}
	}
}

void GenerateProceduralMap(int ratioPercent, int smoothSteps){
	// FillMap(&levels[0], "features", find_block("air"));
	RandomMap(&levels[0], "terrain", 53, find_block("grass"), find_block("water"));


	for(int i = 0; i < smoothSteps; i++){
		SmoothMap(&levels[0], find_block("grass"), find_block("water"));
		// SmoothMap(&levels[0], find_block("grass"), find_block("water"));
	}

	GenerateFlowers(&levels[0], 10);


	DefineCollisions(&levels[0]);

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
	for(int y = 0; y < level->map_size.y; y++){
		for(int x = 0; x < level->map_size.x; x++){
			int surroundTiles = GetSurroundCount(level, (Vector2){x, y}, find_block("grass"));
			if(surroundTiles > 4){
				level->terrain[y][x].block = main;
			}else if(surroundTiles < 4){
				level->terrain[y][x].block = secondary;
			}
		}
	}
}
void RandomMap(LevelComponent *level, char *layer, int ratioPercent, BlockComponent *base, BlockComponent *secondary){
	time_t rawTime;
	struct tm *timeinfo;
	time(&rawTime);
	timeinfo = localtime(&rawTime);
	int worldSeed = timeinfo->tm_sec;
	unsigned long seed;
	seed = 28;
	// seed = worldSeed;
	// seed = SDL_GetTicks() * worldSeed;
	SeedLehmer(seed, 0, 0);
	level->seed = seed;

	RenderTileComponent **specifiedLayer;
	if(strcmp(layer, "terrain") == 0){
		specifiedLayer = level->terrain;
	}else if(strcmp(layer, "features") == 0){
		specifiedLayer = level->features;
	}

	for(int y = 0; y < level->map_size.y; y++){
		for(int x = 0; x < level->map_size.x; x++){
			if(getRnd(0, 102) <= ratioPercent){
				specifiedLayer[y][x].block = base;
			}else{
				specifiedLayer[y][x].block = secondary;
			}
		}
	}
}

void DefineCollisions(LevelComponent *level){
	for(int y = 0; y < level->map_size.y; y++){
		memset(level->collision[y], -1, level->map_size.x * sizeof(int));//Wipe the current line
		for(int x = 0; x < level->map_size.x; x++){
			if(level->features[y][x].block->collisionType != -1){
				level->collision[y][x] = level->features[y][x].block->collisionType;
			}
			if(level->terrain[y][x].block->collisionType != -1){
				level->collision[y][x] = level->terrain[y][x].block->collisionType;
			}
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
			activeLevel->terrain[tile.y][tile.x].block = block;
		}else if(strcmp(block->layer, "feature") == 0){
			activeLevel->features[tile.y][tile.x].block = block;
		}
		activeLevel->collision[tile.y][tile.x] = block->collisionType;//INCOMPLETE
	}
}