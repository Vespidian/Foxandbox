#ifndef TILEMAP_H_
#define TILEMAP_H_

extern int colMap[32][32];
extern int map[32][32];

extern int tilePixelSize;

void LoadImage(SDL_Rect destRect, SDL_Rect sourceRect, SDL_Texture *sourceTexture);

Vector2 GetTileSheetLocation(int tileNum);

void LoadMap(char *fileLoc, int mapArray[][32]);

void DrawMap(SDL_Texture *textureSheet, int mapArray[][32]);

#endif