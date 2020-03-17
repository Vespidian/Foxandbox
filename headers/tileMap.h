#ifndef TILEMAP_H_
#define TILEMAP_H_

extern int colMap[32][32];
extern int map[32][32];
extern int furnitureMap[32][32];
extern int passableMap[32][32];
// extern int passableMap[32][32];

extern int tilePixelSize;

void RenderTextureFromSheet(SDL_Renderer *gRenderer, SDL_Texture *sourceTexture, int tileSheetWidth, int tileWidth, int tileNum, SDL_Rect destRect);

void LoadMap(char *fileLoc, int mapArray[][32]);

void DrawMap(SDL_Texture *textureSheet, int mapArray[][32]);

#endif