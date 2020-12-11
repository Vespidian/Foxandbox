#include "../global.h"
#include "level.h"
#include "level_generation.h"

void FillLevelLayer(LevelObject *level, TileObject **layer, BlockObject *block){
	for(int y = 0; y < level->mapSize.y; y++){
		for(int x = 0; x < level->mapSize.x; x++){
			layer[y][x].block = block->id;
		}
	}
}