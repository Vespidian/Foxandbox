#ifndef TILESHEET_H_
#define TILESHEET_H_

#include "../global.h"
#include "../textures.h"

typedef struct TilesheetObject{
	char *name;
	TextureObject *texture;
	Vector2 tileSize;
}TilesheetObject;


extern TilesheetObject *tilesheets;
extern TilesheetObject undefinedTilesheet;


void InitTilesheets();
void CreateTilesheet(char *name, TextureObject *texture, Vector2 tileSize);
TilesheetObject *FindTilesheet(char *name);

#endif