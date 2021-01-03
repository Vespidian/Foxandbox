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
    const char *path;
    ChunkObject chunk[2][2];//Number of chunks loaded to be changed depending on screen size and via settings
    ChunkObject *chunkBuffer;
    int chunkBufferSize;
}SandboxObject;

extern int tileRenderSize;
extern fVector2 globalOffset;
// extern LevelObject activeLevel;
extern Vector2 mouseTilePos;

void InitLevels();
// LevelObject *NewLevel(char *name, Vector2 size);
// LevelObject *FindLevel(char *name);
// void SetActiveLevel(LevelObject *level);
void RenderLevel();

BlockObject *PlaceBlock(TileObject **layer, BlockObject *block, Vector2 pos, int rotation);
BlockObject *RemoveBlock(TileObject **layer, Vector2 pos);

#endif