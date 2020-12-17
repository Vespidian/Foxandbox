#ifndef LEVEL_H_
#define LEVEL_H_

#include "../entities/block.h"

typedef struct TileObject{
    unsigned int block;
    int zPos;
    int rotation;
    int alpha;
    SDL_Color color;
}TileObject;

typedef struct LevelObject{
    char *name;
    unsigned int id;
    unsigned long seed;
    Vector2 mapSize;
    TileObject **terrain;
    TileObject **features;
    // TileObject **layers[8];
    int **collision;
}LevelObject;

extern int tileRenderSize;
extern Vector2 globalCoordinates;
extern LevelObject activeLevel;
extern Vector2 mouseTilePos;

void InitLevels();
LevelObject *NewLevel(char *name, Vector2 size);
LevelObject *FindLevel(char *name);
void SetActiveLevel(LevelObject *level);
void RenderLevel();

BlockObject *PlaceBlock(TileObject **layer, BlockObject *block, Vector2 pos, int rotation);
BlockObject *RemoveBlock(TileObject **layer, Vector2 pos);

// void LevelMouseInteraction();

#endif