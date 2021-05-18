#ifndef sandbox_generation_H_
#define sandbox_generation_H_

// void FillLevelLayer(LevelObject *level, TileObject **layer, BlockObject *block);
void FillChunk(iVector2 chunk);
void RandomFill(iVector2 chunk, int percentage);
// void IterateCellularAutomata(iVector2 chunk);
void IterateCellularAutomata(ChunkObject *chunk);

void CheckBufferGeneration();
int GetNeighbours(iVector2 coordinate, BlockObject *block);
bool CoordinateConvert(iVector2 coordinateIn, iVector2 *chunkOut, iVector2 *offsetOut);

#endif