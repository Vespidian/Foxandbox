#include "../global.h"
#include "../debug.h"
#include "tilesheet.h"
#include "renderer.h"

TilesheetObject *tilesheets;
unsigned int numTilesheets = 0;
static unsigned int nextID = 0;

TilesheetObject undefinedTilesheet;

void InitTilesheets(){
	undefinedTilesheet = (TilesheetObject){"undefined", -1, undefinedTexture.id, (Vector2){16, 16}};
    DebugLog(D_ACT, "Initialized tilesheet subsystem");
}

TilesheetObject *CreateTilesheet(char *name, TextureObject *texture, Vector2 tileSize){
    tilesheets = realloc(tilesheets, sizeof(TilesheetObject) * (numTilesheets + 1));
    tilesheets[numTilesheets].name = malloc(sizeof(char) * (strlen(name) + 1));
    tilesheets[numTilesheets] = (TilesheetObject){name, nextID, texture->id, tileSize};
    DebugLog(D_ACT, "Created tilesheet id '%d' with name '%s'", nextID, name);
    numTilesheets++;
    nextID++;
    return &tilesheets[numTilesheets - 1];
}

TilesheetObject *CreateRawTilesheet(char *name, char *path, Vector2 tileSize){
    return CreateTilesheet(name, LoadTexture(renderer, path, name), tileSize);
}

TilesheetObject *FindTilesheet(char *name){
    for(int i = 0; i < numTilesheets; i++){
        if(strcmp(tilesheets[i].name, name) == 0){
            return &tilesheets[i];
        }
    }
    return &undefinedTilesheet;
}

TilesheetObject *IDFindTilesheet(unsigned int id){
    for(int i = 0; i < numTilesheets; i++){
        if(tilesheets[i].id == id){
            return &tilesheets[i];
        }
    }
    return &undefinedTilesheet;
}