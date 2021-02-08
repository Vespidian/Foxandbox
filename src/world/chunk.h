#ifndef SANDBOX_H_
#define SANDBOX_H_

#include "block.h"

typedef struct TileObject{
    unsigned int block;
    int zPos;
    int rotation;
    int alpha;
    SDL_Color color;
}TileObject;

typedef struct ChunkObject{// Chunk size is hard coded
    Vector2 position;
    unsigned int lastAccess;
    TileObject tile[4][32][32];// A chunk is a 32x32 section of map with 8 layers and a collision map
    int collision[32][32];
}ChunkObject;

extern const int chunkSize;
extern const int chunkLayers;

extern int tileRenderSize;
extern int chunkTimeoutLimit;

ChunkObject *FindChunk(Vector2 coordinate);
void WriteChunk(ChunkObject *chunk, Vector2 position);
void UnloadChunk(Vector2 coordinate);
void UnloadOldChunks();
void RenderChunk(ChunkObject *chunk, Vector2 position);

//tmp
BlockObject *PlaceBlock(BlockObject *block, Vector2 position, int layer, int rotation);
BlockObject *RemoveBlock(TileObject **layer, Vector2 pos);

#endif