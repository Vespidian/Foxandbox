#ifndef SANDBOX_H_
#define SANDBOX_H_

#include "block.h"

typedef struct TileObject{
    unsigned int block;
    int zPos;
    int rotation;
    int alpha;
    SDL_Color color;
    uint8_t autotile_value;
    uint8_t autotile_value_uncompressed;
}TileObject;

typedef struct ChunkObject{// Chunk size is hard coded
    Vector2 position;
    unsigned int lastAccess;
    TileObject tile[4][32][32];// A chunk is a 32x32 section of map with 8 layers and a collision map
    int collision[32][32];
    bool isGenerated;
}ChunkObject;

extern const int chunk_size;
extern const int num_chunk_layers;

extern int tile_render_size;
extern int chunk_buffer_timeout;

ChunkObject *FindChunk(Vector2 coordinate);
bool CheckChunkExists(Vector2 chunk);
void WriteChunk(ChunkObject *chunk, Vector2 position);
void UnloadChunk(Vector2 coordinate);
void UnloadOldChunks();
void RenderChunk(ChunkObject *chunk, Vector2 position);

//tmp
BlockObject *PlaceBlock(BlockObject *block, Vector2 position, int layer, int rotation);
BlockObject *RemoveBlock(TileObject **layer, Vector2 pos);

#endif