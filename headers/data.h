#ifndef DATA_H_
#define DATA_H_

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <luaconf.h>

char *__strtok_r (char *s, const char *delim, char **save_ptr);

void SeedLehmer(int worldSeed, int x, int y);
extern uint32_t nLehmer;
uint32_t Lehmer32();

int getRnd(int min, int max);

char *strshft_l(char stringShift[128], int shiftBy);

//Predefinitions
extern SDL_Event e;
extern float deltaTime;

void clearScreen(SDL_Renderer *renderer);

extern Vector2 mapOffsetPos;
extern Vector2 playerCoord;
extern Vector2 characterOffset;
extern Vector2 midScreen;
extern TransformComponent mouseTransform;

	
extern int tileSize;
extern bool enableHitboxes;
extern int characterFacing;

extern SDL_Rect windowRect;

extern bool uiInteractMode;

void DrawCharacter(int direction, int numFrames);

extern bool mouseClicked;
extern bool mouseHeld;
extern bool showDebugInfo;
extern bool reachLimit;
extern int reachDistance;

//Lua functions
int num_from_table(lua_State *L, char *field);


//TMP
extern DroppedItemComponent *droppedItems;
void DropItem(ItemComponent *item, int qty, Vector2 pos);
void RenderDroppedItems();

#endif