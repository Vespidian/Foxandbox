#ifndef TILEMAP_H_
#define TILEMAP_H_

extern enum zBufferOrder {RNDRLYR_MAP = 0, RNDRLYR_PLAYER = 10, RNDRLYR_UI = 20, RNDRLYR_INV_ITEMS = 25, RNDRLYR_TEXT = 30};

extern int colMap[32][32];
extern int map[32][32];
extern int map1[32][32];
extern int furnitureMap[32][32];
extern int passableMap[32][32];

extern int tempMap1[32][32];
extern int tempMap2[32][32];


extern int customMap[32][32];
// extern int passableMap[32][32];

extern int tilePixelSize;

// int RenderTextureFromSheet(SDL_Renderer *gRenderer, SDL_Texture *sourceTexture, int tileSheetWidth, int tileSheetHeight, int tileWidth, int tileNum, SDL_Rect destRect);
int AddToRenderQueue(SDL_Renderer *gRenderer, WB_Tilesheet tileSheet, int tileNum, SDL_Rect destRect, int zPos);
// int RenderTextureFromSheet(SDL_Renderer *gRenderer, WB_Tilesheet tileSheet, int tileNum, SDL_Rect destRect);
// int RenderTextureFromSheet();
void RenderUpdate();
void RenderTextureInWorld(SDL_Renderer *renderer, WB_Tilesheet sheet, int tile, SDL_Rect rect, int zPos);

int LoadMap(char *fileLoc, int mapArray[][32]);

// void DrawMap(SDL_Texture *textureSheet, int sheetWidth, int mapArray[][32]);
void DrawMap(WB_Tilesheet tileSheet, int mapArray[][32], int zPos);

void TextExtrapolate(int map[][32]);

void ExtrapolateMap(char *file, int map1[][32], int map2[][32]);

void SetupRenderFrame();

#endif