#ifndef WORLD_H_
#define WORLD_H_

#include "chunk.h"

typedef struct SandboxObject{
    char *name;
    bool isActive;
    unsigned long seed;
    ChunkObject *chunkBuffer;//Number of chunks loaded to be changed depending on screen size and via settings
    int chunkBufferSize;
}SandboxObject;

extern int chunkLoadRadius;

extern SandboxObject activeSandbox;
extern SandboxObject undefinedSandbox;

void InitSandboxes();
void RenderSandbox();

void UnloadSandbox();
void ReadSandbox(char *name);
extern const char savePath[];

extern Vector2 globalOffset;
extern Vector2 globalCoordinates;
extern Vector2 mouseTilePos;
extern Vector2 mouseGlobalTilePos;

#endif