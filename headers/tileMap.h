#ifndef TILEMAP_H_
#define TILEMAP_H_

extern int colMap[32][32];
extern int map[32][32];
extern int map1[32][32];
extern int furnitureMap[32][32];
extern int passableMap[32][32];

extern int tempMap1[32][32];
extern int tempMap2[32][32];
// extern int passableMap[32][32];

extern int tilePixelSize;

int RenderTextureFromSheet(SDL_Renderer *gRenderer, SDL_Texture *sourceTexture, int tileSheetWidth, int tileSheetHeight, int tileWidth, int tileNum, SDL_Rect destRect);

void LoadMap(char *fileLoc, int mapArray[][32]);

void DrawMap(SDL_Texture *textureSheet, int sheetWidth, int mapArray[][32]);


void TextExtrapolate(int map[][32]);

void ExtrapolateMap(char *file, int map1[][32], int map2[][32]);

#endif