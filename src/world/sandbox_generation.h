#ifndef sandbox_generation_H_
#define sandbox_generation_H_

// void FillLevelLayer(LevelObject *level, TileObject **layer, BlockObject *block);
void FillChunk(Vector2_i chunk);
void RandomFill(Vector2_i chunk, int percentage);
// void IterateCellularAutomata(Vector2_i chunk);
void IterateCellularAutomata(ChunkObject *chunk);

void CheckBufferGeneration();
int GetNeighbours(Vector2_i coordinate, BlockObject *block);
bool CoordinateConvert(Vector2_i coordinateIn, Vector2_i *chunkOut, Vector2_i *offsetOut);

#endif