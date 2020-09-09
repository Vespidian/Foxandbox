#ifndef TILEMAP_H_
#define TILEMAP_H_

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <luaconf.h>

extern enum zBufferOrder {RNDRLYR_MAP = 0, RNDRLYR_PLAYER = 5, RNDRLYR_UI = 20, RNDRLYR_INV_ITEMS = 25, RNDRLYR_TEXT = 30};

extern const int MAXLEVELSIZE;

extern LevelComponent *activeLevel;

extern LevelComponent *levels;
extern int numLevels;

extern int renderItemIndex;
extern int tilePixelSize;
extern const int tileStretchSize;

extern WB_Tilesheet *tilesheets;
WB_Tilesheet *find_tilesheet(char *name);
int register_tilesheet(lua_State *L);

int LoadMap(char *fileLoc, RenderTileComponent mapArray[][32]);
int LoadDataMap(char *fileLoc, int mapArray[][32]);

//Rendering system
void SetupRenderFrame();
int AddToRenderQueue(SDL_Renderer *renderer, WB_Tilesheet tileSheet, int tileNum, SDL_Rect destRect, int alpha, int zPos);
void RenderTextureInWorld(SDL_Renderer *renderer, WB_Tilesheet sheet, int tile, SDL_Rect rect, int zPos);
void RenderUpdate();


void RenderLevel(LevelComponent *level);

void DrawLevel();

extern BlockComponent undefinedBlock;
extern BlockComponent *blockData;
extern int numBlocks;

extern AutotileComponent *autotileData;
extern int numAutotiles;

BlockComponent *find_block(char *name);

// extern LevelComponent_t *levels_tmp;
int LoadLevel(char *path);
int SaveLevel(LevelComponent *level, char *path);

//Lua functions
int register_block(lua_State *L);
int populate_autotile(lua_State *L);

#endif