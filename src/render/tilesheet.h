#ifndef TILESHEET_H_
#define TILESHEET_H_

#include "../global.h"
#include "../textures.h"

typedef struct TilesheetObject{
	char *name;
	unsigned int id;
	unsigned int texture;
	Vector2 tileSize;
}TilesheetObject;


extern TilesheetObject *tilesheets;
extern TilesheetObject undefinedTilesheet;


void InitTilesheets();
TilesheetObject *NewTilesheet(char *name, TextureObject *texture, Vector2 tileSize);
TilesheetObject *NewRawTilesheet(char *name, char *path, Vector2 tileSize);
TilesheetObject *FindTilesheet(char *name);
TilesheetObject *IDFindTilesheet(unsigned int id);

#endif