#include <stdio.h>
#include <stdbool.h>
#include <time.h>

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
	FillMap(buildLayer);
	// RandomMap(buildLayer, ratioPercent);
	for(int i = 0; i < smoothSteps; i++){
		SmoothMap();
	}
	AutotileMap(buildLayer);
}

void AutotileMap(RenderTileComponent map[][32]){
	/*
	 1  |  2  | 4
	____|_____|____
	 8  | /// | 16
	____|_____|____
	32  | 64  |128
	*/
	int correlationArray[] = {255, 2, 8, 10, 11, 16, 18, 22, 24, 26, 27, 30, 31, 64, 66, 72, 74, 75, 80, 82, 86, 88, 90, 91, 94, 95, 104, 106, 107, 120, 122, 123, 126, 127, 208, 210, 214, 216, 218, 219, 222, 223, 248, 250, 251, 254, 0};
	for(int y = 0; y < 32; y++){
		for(int x = 0; x < 32; x++){
			int adjacentCaseNumber = 0;			
			int cornerCaseNumber = 0;	
			int finalCaseNumber = 0;
			//Auto Tiling

			int yNeg = (y - 1) * (y != 0);
			int yPos = (y + 1) * (y != 31);
			int xNeg = (x - 1) * (x != 0);
			int xPos = (x + 1) * (x != 31);
			//Corner
			bool bottomRight = (map[yNeg][xPos].block->tile < WATER);
			bool bottomLeft = (map[yNeg][xNeg].block->tile < WATER);
			bool topRight = (map[yPos][xPos].block->tile < WATER);
			bool topLeft = (map[yPos][xNeg].block->tile < WATER);
			//Adjacent
			bool bottom = (map[yNeg][x].block->tile < WATER);
			bool right = (map[y][xPos].block->tile < WATER);
			bool left = (map[y][xNeg].block->tile < WATER);
			bool top = (map[yPos][x].block->tile < WATER);

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
				cornerCaseNumber += topLeft << 0;//CRN
				adjacentCaseNumber += top << 1;//ADJ
				cornerCaseNumber += topRight << 2;//CRN
				adjacentCaseNumber += left << 3;//ADJ
				adjacentCaseNumber += right << 4;//ADJ
				cornerCaseNumber += bottomLeft << 5;//CRN
				adjacentCaseNumber += bottom << 6;//ADJ
				cornerCaseNumber += bottomRight << 7;//CRN
				finalCaseNumber = adjacentCaseNumber + cornerCaseNumber;
				//Order the 47 possible options in a compact way
				for(int i = 0; i < 47; i++){
					if(finalCaseNumber == correlationArray[i]){
						finalCaseNumber = i;
						break;
					}
				}
				if(finalCaseNumber == GRASS){
					buildLayer[y][x].block = autotile[0].baseBlock;
				}else if(finalCaseNumber == WATER){
					buildLayer[y][x].block = autotile[0].subBlock;
				}else{
					buildLayer[y][x].block = &autotile[0].auto_block[finalCaseNumber - 1];
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
void RandomMap(RenderTileComponent map[][32], int ratioPercent){
	if(doRandomGen){
		time_t rawTime;
		struct tm *timeinfo;
		time(&rawTime);
		timeinfo = localtime(&rawTime);
		int worldSeed = timeinfo->tm_sec;
		SeedLehmer(worldSeed, 0, 0);
	}else{
		SeedLehmer(1, 0, 0);
	}
	for(int y = 0; y < 32; y++){
		for(int x = 0; x < 32; x++){
			if(getRnd(0, 100) <= ratioPercent){
				map[y][x].block = find_block("grass");
			}else{
				map[y][x].block = find_block("water");
			}
		}
	}
}

void TileMapEdit(RenderTileComponent tileMap[][32], Vector2 pos, BlockComponent *block, bool collide){
	
	tileMap[pos.y][pos.x].block = block;
	
	AutotileMap(buildLayer);
}

void PlaceBlock(){
	
}