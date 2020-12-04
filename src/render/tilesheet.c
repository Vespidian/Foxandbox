#include "../global.h"
#include "../debug.h"
#include "tilesheet.h"
#include "renderer.h"

TilesheetObject *tilesheets;
int numTilesheets = 0;

TilesheetObject undefinedTilesheet;

void InitTilesheets(){
	undefinedTilesheet = (TilesheetObject){"undefined", &undefinedTexture, (Vector2){16, 16}};
    DebugLog(D_ACT, "Initialized tilesheet subsystem");
}

void CreateTilesheet(char *name, TextureObject *texture, Vector2 tileSize){
    tilesheets = realloc(tilesheets, sizeof(TilesheetObject) * (numTilesheets + 1));
    tilesheets[numTilesheets].name = malloc(sizeof(char) * strlen(name));
    tilesheets[numTilesheets] = (TilesheetObject){name, texture, tileSize};
    DebugLog(D_ACT, "Tilesheet created id '%d' with name '%s'", numTilesheets, name);
    numTilesheets++;
}

void CreateRawTilesheet(char *name, char *path, Vector2 tileSize){
    CreateTilesheet(name, LoadTexture(renderer, path, name), tileSize);
}

TilesheetObject *FindTilesheet(char *name){
    for(int i = 0; i < numTilesheets; i++){
        if(strcmp(tilesheets[i].name, name) == 0){
            return &tilesheets[i];
        }
    }
    return &undefinedTilesheet;
}