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

RenderTileComponent buildLayer_tmp[32][32] = {};
enum types {GRASS = 0, WATER = 47};
bool doRandomGen = true;

int GetSurroundCount(Vector2 tile){
	int surroundCount = 0;
	for(int y = tile.y - 1; y <= tile.y + 1; y++){
		for(int x = tile.x - 1; x <= tile.x + 1; x++){
			if(x >= 0 && x < 32 && y >= 0 && y < 32){
				if(x != tile.x || y != tile.y){
					if(buildLayer_tmp[y][x].block->tile < WATER){
						surroundCount++;
					}
				}
			}
		}
	}
	// printf("%d\n", surr	oundCount);
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
	// FillMap(buildLayer);
	RandomMap(buildLayer, ratioPercent, 2, find_block("grass"), find_block("water"));
	// RandomMap(buildLayer, ratioPercent, "grass", "water", NULL);
	for(int i = 0; i < smoothSteps; i++){
		SmoothMap();
	}
	buildLayer[6][6].block = find_block("nylium");
	AutotileMap(buildLayer, autotileData[0]);
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
				// 	buildLayer[y][x].block = autotileData[0].baseBlock;
				// }else if(finalCaseNumber == WATER){
				// 	buildLayer[y][x].block = autotileData[0].subBlock;
				// }
				if(finalCaseNumber != 0 && finalCaseNumber != 255){
					buildLayer[y][x].block = &autotile.auto_block[finalCaseNumber - 1];
				}
			}

			if(buildLayer[y][x].block->tile == WATER){
				colMap[y][x] = 0;
			}else if(buildLayer[y][x].block->tile > GRASS){
				colMap[y][x] = -1;
			}

			}
		}
	}
}

void SmoothMap(){
	for(int y = 0; y < 32; y++){
		for(int x = 0; x < 32; x++){
			if(buildLayer[y][x].block->tile < WATER){
				buildLayer_tmp[y][x].block = find_block("grass");
			}else{
				buildLayer_tmp[y][x].block = find_block("water");
			}
		}
	}
	for(int y = 0; y < 32; y++){
		for(int x = 0; x < 32; x++){
			int surroundTiles = GetSurroundCount((Vector2){x, y});
			if(surroundTiles > 4){
				buildLayer[y][x].block = find_block("grass");
				buildLayer_tmp[y][x].block = find_block("grass");
			}else if(surroundTiles < 4){
				buildLayer[y][x].block = find_block("water");
				buildLayer_tmp[y][x].block = find_block("water");
			}
		}
	}

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

void TileMapEdit(RenderTileComponent tileMap[][32], Vector2 pos, BlockComponent *block, bool collide){
	
	tileMap[pos.y][pos.x].block = block;
	
	AutotileMap(buildLayer, autotileData[0]);
}

void PlaceBlock(){
	
}