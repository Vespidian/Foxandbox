#ifndef sandbox_generation_H_
#define sandbox_generation_H_

// void FillLevelLayer(LevelObject *level, TileObject **layer, BlockObject *block);
void FillChunk(Vector2 chunk);
void RandomFill(Vector2 chunk, int percentage);
// void IterateCellularAutomata(Vector2 chunk);
void IterateCellularAutomata(ChunkObject *chunk);

void CheckBufferGeneration();
int GetNeighbours(Vector2 coordinate, BlockObject *block);
bool CoordinateConvert(Vector2 coordinateIn, Vector2 *chunkOut, Vector2 *offsetOut);

#endif