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
#include "headers/level_systems.h"
#include "headers/map_generation.h"
#include "headers/inventory.h"
#include "headers/initialize.h"
#include "headers/render_systems.h"
#include "headers/lua_systems.h"

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
			level->terrain[y][x].rotation = 0;
			level->features[y][x].rotation = 0;
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
					if(&level->terrain[y][x].block->item->name == &type->item->name){
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
			}
		}
	}
}

void GenerateProceduralMap(LevelComponent *level, int ratioPercent, int smoothSteps){
	RandomMap(level, "terrain", 53, find_block("grass"), find_block("water"));


	for(int i = 0; i < smoothSteps; i++){
		SmoothMap(level, find_block("grass"), find_block("water"));
	}

	GenerateFlowers(level, 10);
	DefineCollisions(level);
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
	// time_t rawTime;
	// struct tm *timeinfo;
	// time(&rawTime);
	// timeinfo = localtime(&rawTime);
	// int worldSeed = timeinfo->tm_sec;
	unsigned long seed;
	seed = 831130583115427;
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

int CalculateBlockRotation(Vector2 player, Vector2 tile){	
	int rotation = 0;
	Vector2 playerDir = {0, 0};
	if(abs(tile.x - player.x) > abs(tile.y - player.y)){
		playerDir.x = 1;
	}
	if(abs(tile.x - player.x) <= abs(tile.y - player.y)){
		playerDir.y = tile.y > player.y ? 0 : 1;
	}else if(tile.x > player.x){
		playerDir.y = 1;
	}
	rotation = playerDir.y << 1;
	rotation += playerDir.x;
	return rotation;
}

void PlaceBlock(RenderTileComponent **layer, Vector2 tile, BlockComponent *block){
	layer[tile.y][tile.x].block = block;
	if(block->allowRotation){
		layer[tile.y][tile.x].rotation = CalculateBlockRotation(character.transform.tilePos, tile);
	}else{
		layer[tile.y][tile.x].rotation = 0;
	}
	DebugLog(D_ACT, "placed tile '%s' at %d,%d", layer[tile.y][tile.x].block->item->name, tile.x, tile.y);
	activeLevel->collision[tile.y][tile.x] = block->collisionType;//INCOMPLETE

}