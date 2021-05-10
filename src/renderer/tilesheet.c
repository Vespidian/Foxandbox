#include "../global.h"
#include "../debug.h"
#include "tilesheet.h"
#include "renderer.h"

TilesheetObject *tilesheets;
unsigned int num_tilesheets = 0;
static unsigned int nextID = 0;

TilesheetObject undefined_tilesheet;

void InitTilesheets(){
	undefined_tilesheet = (TilesheetObject){"undefined", -1, undefined_texture.id, (Vector2){16, 16}};
    DebugLog(D_ACT, "Initialized tilesheet subsystem");
}

TilesheetObject *NewTilesheet(char *name, TextureObject *texture, Vector2 tileSize){
    tilesheets = realloc(tilesheets, sizeof(TilesheetObject) * (num_tilesheets + 1));
    tilesheets[num_tilesheets].name = malloc(sizeof(char) * (strlen(name) + 1));
    tilesheets[num_tilesheets] = (TilesheetObject){name, nextID, texture->id, tileSize};
    DebugLog(D_ACT, "Created tilesheet id '%d' with name '%s'", nextID, name);
    num_tilesheets++;
    nextID++;
    return &tilesheets[num_tilesheets - 1];
}

TilesheetObject *NewRawTilesheet(char *name, char *path, Vector2 tileSize){
    return NewTilesheet(name, LoadTexture(renderer, path, name), tileSize);
}

TilesheetObject *FindTilesheet(char *name){
    for(int i = 0; i < num_tilesheets; i++){
        if(strcmp(tilesheets[i].name, name) == 0){
            return &tilesheets[i];
        }
    }
    return &undefined_tilesheet;
}

TilesheetObject *IDFindTilesheet(unsigned int id){
    for(int i = 0; i < num_tilesheets; i++){
        if(tilesheets[i].id == id){
            return &tilesheets[i];
        }
    }
    return &undefined_tilesheet;
}