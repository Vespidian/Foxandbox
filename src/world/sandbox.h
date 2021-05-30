#ifndef WORLD_H_
#define WORLD_H_

#include "chunk.h"

typedef struct SandboxObject{
    char *name;
    bool isActive;
    unsigned long seed;
    ChunkObject *chunk_buffer;//Number of chunks loaded to be changed depending on screen size and via settings
    int chunk_buffer_size;
}SandboxObject;

extern int chunk_load_radius;

extern SandboxObject active_sandbox;
extern SandboxObject undefined_sandbox;

void InitSandboxes();
void RenderSandbox();

void UnloadSandbox();
void ReadSandbox(char *name);
extern const char save_path[];

extern Vector2_i player_coordinate_offset;
extern Vector2_i player_coordinate;
extern Vector2_i mouse_tile_pos;
extern Vector2_i mouse_global_tile_pos;

#endif