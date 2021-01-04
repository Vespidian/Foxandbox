#ifndef LEVEL_H_
#define LEVEL_H_

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
    TileObject tile[2][4][4];// A chunk is a 32x32 section of map with 8 layers and a collision map
    int collision[4][4];
}ChunkObject;

typedef struct SandboxObject{
    char *name;
    unsigned int id;
    unsigned long seed;
    char *path;
    // ChunkObject chunk[3][3];
    ChunkObject *chunkBuffer;//Number of chunks loaded to be changed depending on screen size and via settings
    int chunkBufferSize;
}SandboxObject;

extern int tileRenderSize;
extern fVector2 globalOffset;
extern SandboxObject activeSandbox;
extern Vector2 mouseTilePos;

void InitLevels();
// void SetActiveLevel(LevelObject *level);
// void RenderLevel();
void RenderSandbox();

BlockObject *PlaceBlock(TileObject **layer, BlockObject *block, Vector2 pos, int rotation);
BlockObject *RemoveBlock(TileObject **layer, Vector2 pos);


//TMP
void RenderChunk(ChunkObject *chunk, Vector2 position);
ChunkObject *FindChunk(Vector2 coordinate);
void FillChunk(Vector2 coordinate);

#endif