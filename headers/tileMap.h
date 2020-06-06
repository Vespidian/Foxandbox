#ifndef TILEMAP_H_
#define TILEMAP_H_

extern enum zBufferOrder {RNDRLYR_MAP = 0, RNDRLYR_PLAYER = 5, RNDRLYR_UI = 20, RNDRLYR_INV_ITEMS = 25, RNDRLYR_TEXT = 30};

extern int colMap[32][32];

extern RenderTileComponent map[32][32];
extern RenderTileComponent map1[32][32];
extern RenderTileComponent furnitureMap[32][32];
extern RenderTileComponent passableMap[32][32];
extern RenderTileComponent customMap[32][32];

extern RenderTileComponent randArray[32][32];


extern int tempMap1[32][32];
extern int tempMap2[32][32];


// extern int passableMap[32][32];



extern int tilePixelSize;

int AddToRenderQueue(SDL_Renderer *gRenderer, WB_Tilesheet tileSheet, int tileNum, SDL_Rect destRect, int zPos);
void RenderUpdate();
void RenderTextureInWorld(SDL_Renderer *renderer, WB_Tilesheet sheet, int tile, SDL_Rect rect, int zPos);

int LoadMap(char *fileLoc, RenderTileComponent mapArray[][32]);
int LoadDataMap(char *fileLoc, int mapArray[][32]);
// void DrawMap(SDL_Texture *textureSheet, int sheetWidth, RenderTileComponent mapArray[][32]);
void DrawMap(WB_Tilesheet tileSheet, RenderTileComponent mapArray[][32], int zPos);

void TextExtrapolate(RenderTileComponent map[][32]);

void ExtrapolateMap(char *file, RenderTileComponent map1[][32], RenderTileComponent map2[][32]);

void SetupRenderFrame();

void DrawLevel();

#endif