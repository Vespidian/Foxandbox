#include "../global.h"
#include "../utility.h"
#include "chunk.h"
#include "sandbox.h"
#include "sandbox_generation.h"
#include "generation/perlin.h"


/*void CoordinateConvert(Vector2 coordinateIn, Vector2 *chunkOut, Vector2 *offsetOut){
	offsetOut->x = coordinateIn.x % chunk_size;
    chunkOut->x = coordinateIn.x / chunk_size;
	offsetOut->y = coordinateIn.y % chunk_size;
	chunkOut->y = coordinateIn.y / chunk_size;
	// offsetOut = {coordinateIn.x % chunk_size, coordinateIn.y % chunk_size};
    // chunkOut = {coordinateIn.x / chunk_size, coordinateIn.y / chunk_size};
    //Negative coordinates
    if(coordinateIn.x < 0){
        offsetOut->x = (coordinateIn.x + 1) % chunk_size + chunk_size - 1;
        chunkOut->x = (coordinateIn.x + 1) / chunk_size;
    }
    if(coordinateIn.y < 0){
        offsetOut->y = (coordinateIn.y + 1) % chunk_size + chunk_size - 1;
        chunkOut->y = (coordinateIn.y + 1) / chunk_size;
    }
}*/

bool CoordinateConvert(Vector2 coordinateIn, Vector2 *chunkOut, Vector2 *offsetOut){
	/* Prototyping
	int chunk;
    int offset;
    if(input >= 0){
        chunk = (input + 1) / chunk_size;
        
        offset = (input) % chunk_size;
    }else{
        chunk = ((input + 1) - chunk_size) / chunk_size;
        
        offset = (chunk_size * -chunk + (input)) % chunk_size;
    }
    printf("%d\n%d", chunk, offset);*/
	
    if(coordinateIn.x >= 0){
    	chunkOut->x = coordinateIn.x / chunk_size;// Positive chunk
		offsetOut->x = coordinateIn.x % chunk_size;// Positive offset
    }else{
		chunkOut->x = ((coordinateIn.x + 1) - chunk_size) / chunk_size;// Negative chunk
		offsetOut->x = (chunk_size * -chunkOut->x + coordinateIn.x) % chunk_size;// Negative offset
	}

    if(coordinateIn.y >= 0){
		chunkOut->y = coordinateIn.y / chunk_size;// Positive chunk
		offsetOut->y = coordinateIn.y % chunk_size;// Positive offset
    }else{
		chunkOut->y = ((coordinateIn.y + 1) - chunk_size) / chunk_size;// Negative chunk
		offsetOut->y = (chunk_size * -chunkOut->y + coordinateIn.y) % chunk_size;// Negative offset
	}
	if(!CheckChunkExists(*chunkOut)){
		*chunkOut = (Vector2){0, 0};
		return false;
	}
	return true;
}

Vector2 Coordinate2Chunk(Vector2 coordinate){
    Vector2 chunk;
    CoordinateConvert(coordinate, &chunk, NULL);
	return chunk;
}

Vector2 Coordinate2Offset(Vector2 coordinate){
	Vector2 offset;
    CoordinateConvert(coordinate, NULL, &offset);
	return offset;
}

Vector2 ToCoordinate(Vector2 chunk, Vector2 offset){
	// Vector2 coordinate = {(chunk.x * chunk_size + offset.x), (chunk.y * chunk_size + offset.y)};

	// return (Vector2){(chunk.x * chunk_size + offset.x), (chunk.y * chunk_size + offset.y)};
	// printf("offset intput: %d chunk input: %d coordinate output: %d\n", offset.y, chunk.y, coordinate.y);
	// return coordinate;
	return (Vector2){(chunk.x * chunk_size + offset.x), (chunk.y * chunk_size + offset.y)};
}



/*void FillLevelLayer(LevelObject *level, TileObject **layer, BlockObject *block){
	for(int y = 0; y < level->mapSize.y; y++){
		for(int x = 0; x < level->mapSize.x; x++){
			layer[y][x].block = block->id;
		}
	}
}*/
void FillChunk(Vector2 chunk){
    for(int y = 0; y < chunk_size; y++){
        for(int x = 0; x < chunk_size; x++){
            // printf("here\n");
            FindChunk(chunk)->tile[0][y][x].block = FindBlock("grass")->id;
        }
    }
}

void RandomFill(Vector2 chunk, int percentage){
	for(int y = 0; y < chunk_size; y++){
		for(int x = 0; x < chunk_size; x++){
			int chance = Rand(0, 100);
			if(chance < percentage){
				FindChunk(chunk)->tile[0][y][x].block = FindBlock("water")->id;
			}else{
				FindChunk(chunk)->tile[0][y][x].block = FindBlock("grass")->id;
			}
		}
	}
}


// int GetSurroundCount(Vector2 tile, BlockComponent *type){
// 	int surroundCount = 0;
// 	for(int y = tile.y - 1; y <= tile.y + 1; y++){
// 		for(int x = tile.x - 1; x <= tile.x + 1; x++){
// 			if(x >= 0 && x <= chunk_size && y >= 0 && y <= chunk_size){
// 				// BlockComponent *block;
// 				// if(x == level->map_size.x || y == level->map_size.y){
// 				// 	block = find_block("air");
// 				// }else{
// 				// 	block = level->terrain[y][x].block;
// 				// }
// 				if(x != tile.x || y != tile.y){
// 					if(&block->item->name == &type->item->name){
// 						surroundCount++;
// 					}
// 				}
// 			}
// 		}
// 	}
// 	return surroundCount;
// }


int GetNeighbours(Vector2 coordinate, BlockObject *block){
	int count = 0;
	Vector2 chunk;
	Vector2 offset;

	for(int y = coordinate.y - 1; y <= coordinate.y + 1; y++){
		for(int x = coordinate.x - 1; x <= coordinate.x + 1; x++){
			if(CoordinateConvert((Vector2){x, y}, &chunk, &offset)){
				if(y != coordinate.y || x != coordinate.x){
					if(FindChunk(chunk)->tile[0][offset.y][offset.x].block == block->id){
						count++;
					}
				}
			}
		}
	}
	return count;
}

// void IterateCellularAutomata(Vector2 chunk){
void IterateCellularAutomata(ChunkObject *chunk){
	// ChunkObject *chunk_data = FindChunk(chunk);
	if(!chunk->isGenerated){
		Vector2 position;
		for(int y = 0; y < chunk_size; y++){
			for(int x = 0; x < chunk_size; x++){
				position = ToCoordinate(chunk->position, (Vector2){x, y});
				int surround = GetNeighbours(position, FindBlock("grass"));
				if(surround > 4){
					// chunk_data->tile[0][y][x].block = FindBlock("water")->id;
					// FindChunk(chunk->position)->tile[0][y][x].block = FindBlock("water")->id;
					chunk->tile[0][y][x].block = FindBlock("water")->id;
				// }else if(surround < 4){
				}else{
					// chunk_data->tile[0][y][x].block = FindBlock("grass")->id;
					// FindChunk(chunk->position)->tile[0][y][x].block = FindBlock("grass")->id;
					chunk->tile[0][y][x].block = FindBlock("grass")->id;
				}
			}
		}
	}

	// Vector2 coordinate = {-195, 0};
	// chunk = (Vector2){1, 0};
	// Vector2 offset = {29, 0};
	// CoordinateConvert(coordinate, &chunk, &offset);
	// // coordinate = ToCoordinate(chunk, offset);
	// printf("\nchunk:%d\noffset:%d\n", chunk.x, offset.x);
	// printf("\n%d\n", coordinate.x);




	// printf("%d\n", ToCoordinate((Vector2){-1, 0}, (Vector2){0, 0}).x);
	// printf("%d\n", ToCoordinate((Vector2){0, 0}, (Vector2){31, 0}).x);
	// printf("%d\n", ToCoordinate((Vector2){1, 0}, (Vector2){31, 0}).x);
}

void CalculatePerlinBase(ChunkObject *chunk){
	Vector2 position = {0, 0};
	for(int y = 0; y < chunk_size; y++){
		for(int x = 0; x < chunk_size; x++){
			position = ToCoordinate(chunk->position, (Vector2){x, y});
			int value = Perlin_Get2d(position.x, position.y, 0.05, 2) * 1024;
			// printf("%d\n", value);
			if(value < 600){
				chunk->tile[0][y][x].block = FindBlock("grass")->id;
			// }else if(value > 500 && value < 545){
				// chunk->tile[0][y][x].block = FindBlock("sand")->id;
			}else{
				chunk->tile[0][y][x].block = FindBlock("water")->id;
			}
		}
	}
}

void StepCellularAutomata(ChunkObject *chunk){
	// if(!chunk->isGenerated){
		uint8_t surround = 0;
		Vector2 position = {0, 0};
		Vector2 chunk_pos = {0, 0};
		Vector2 offset_pos = {0, 0};
		// BlockObject *grass_block = FindBlock("grass");
		// BlockObject *water_block = FindBlock("water");

		for(int y = 0; y < chunk_size; y++){
			for(int x = 0; x < chunk_size; x++){
				position = ToCoordinate(chunk->position, (Vector2){x, y});
				// printf("%d, %d\n", position.x, position.y);
				surround = GetNeighbours(position, FindBlock("water"));
				if(surround <= 2){
					if(chunk->tile[0][y][x].block == FindBlock("water")->id){
						CoordinateConvert((Vector2){position.x, position.y + 1}, &chunk_pos, &offset_pos);
						chunk->tile[0][y][x].block = FindChunk(chunk_pos)->tile[0][offset_pos.y][offset_pos.x].block;
						// chunk->tile[0][y][x].block = chunk->tile[0][y][x].block;
					}
				}

				// int value = Perlin_Get2d(position.x, position.y, 0.05, 3) * 1024;
				// printf("%d\n", value);
				// if(value < 600){
				// 	chunk->tile[0][y][x].block = FindBlock("grass")->id;
				// }else{
				// 	chunk->tile[0][y][x].block = FindBlock("water")->id;
				// }
			}
		}
	// }
}

void CheckBufferGeneration(){
	for(int i = 0; i < active_sandbox.chunkBufferSize; i++){
		if(!active_sandbox.chunkBuffer[i].isGenerated){// To be moved to chunk unloading loop
	    	RandomFill(active_sandbox.chunkBuffer[i].position, 50);
			// for(int j = 0; j < 6; j++){
				// IterateCellularAutomata(&active_sandbox.chunkBuffer[i]);
				CalculatePerlinBase(&active_sandbox.chunkBuffer[i]);
				StepCellularAutomata(&active_sandbox.chunkBuffer[i]);
				// StepCellularAutomata(&active_sandbox.chunkBuffer[i]);
				// StepCellularAutomata(&active_sandbox.chunkBuffer[i]);
				// StepCellularAutomata(&active_sandbox.chunkBuffer[i]);
				// StepCellularAutomata(&active_sandbox.chunkBuffer[i]);
				// StepCellularAutomata(&active_sandbox.chunkBuffer[i]);
			// }
			active_sandbox.chunkBuffer[i].isGenerated = true;
		}
	}
}