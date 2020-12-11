#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "../global.h"
#include "../entities/block.h"
#include "../render/renderer.h"
#include "level.h"

const int MAXLEVELSIZE = 512;

LevelObject *levels;
unsigned int numLevels = 0;
static unsigned int nextID = 0;
LevelObject activeLevel;


Vector2 globalCoordinates = {0, 0};
int tileRenderSize = 64;

void InitLevels(){
    NewBlock(NewItem("air", FindTilesheet("default_tilesheet"), 0), FindItem("air"), FindTilesheet("default_tilesheet"), 0, false);
    DebugLog(D_ACT, "Initialized level subsystem");
}

LevelObject *NewLevel(char *name, Vector2 size){
    if(size.x > MAXLEVELSIZE){
        size.x = MAXLEVELSIZE;
    }
    if(size.y > MAXLEVELSIZE){
        size.y = MAXLEVELSIZE;
    }

    levels = realloc(levels, sizeof(LevelObject) * (numLevels + 1));
    levels[numLevels].name = malloc(sizeof(char) * (strlen(name) + 1));
    levels[numLevels] = (LevelObject){.name = name, .id = nextID, .seed = 0, .mapSize = (Vector2)size};

    levels[numLevels].terrain = malloc(sizeof(TileObject) * (size.y + 1));
    levels[numLevels].features = malloc(sizeof(TileObject) * (size.y + 1));
    levels[numLevels].collision = malloc(sizeof(int) * (size.y + 1));

    TileObject templateTile = {FindBlock("air")->id, 0, 0, 255, (SDL_Color){255, 255, 255}};
    for(int y = 0; y < size.y; y++){
        levels[numLevels].terrain[y] = malloc(sizeof(TileObject) * (size.x + 1));
        levels[numLevels].features[y] = malloc(sizeof(TileObject) * (size.x + 1));
        levels[numLevels].collision[y] = malloc(sizeof(int) * (size.x + 1));

        memset(levels[numLevels].collision[y], -1, sizeof(int) * (size.x));

        for(int x = 0; x < size.x; x++){
            levels[numLevels].terrain[y][x] = (TileObject)templateTile;
            levels[numLevels].features[y][x] = (TileObject)templateTile;
        }
    }
    numLevels++;
    nextID++;
    return &levels[numLevels - 1];
}

LevelObject *FindLevel(char *name){
    for(int i = 0; i < numLevels; i++){
        if(strcmp(levels[i].name, name) == 0){
            return &levels[i];
        }
    }
    return NULL;
}

void SetActiveLevel(LevelObject *level){
    activeLevel = *level;
}

void RenderLevel(){ // Renders 'activeLevel'
    if(activeLevel.name != NULL){
        SDL_Rect levelRect = {globalCoordinates.x, globalCoordinates.y, activeLevel.mapSize.x * tileRenderSize, activeLevel.mapSize.y * tileRenderSize};
        PushRender_Tilesheet(renderer, FindTilesheet("builtin"), 0, levelRect, RNDR_LEVEL - 1);

        SDL_Rect tile = {0, 0, tileRenderSize, tileRenderSize};
        //Start render loop at top of window unless at level top edge. Loop until bottom of window unless at bottom level edge
        for(int y = (globalCoordinates.y / tileRenderSize - 1) * ((globalCoordinates.y / tileRenderSize - 1) > 0); y < ((globalCoordinates.y + SCREEN_HEIGHT) / tileRenderSize + 1) && y < activeLevel.mapSize.y; y++){
            for(int x = (globalCoordinates.x / tileRenderSize - 1) * ((globalCoordinates.x / tileRenderSize - 1) > 0); x < ((globalCoordinates.x + SCREEN_WIDTH) / tileRenderSize + 1) && x < activeLevel.mapSize.x; x++){
                tile.x = (x * tileRenderSize) - globalCoordinates.x;
                tile.y = (y * tileRenderSize) - globalCoordinates.y;
                BlockObject *terrainBlock = IDFindBlock(activeLevel.terrain[y][x].block);
                BlockObject *featureBlock = IDFindBlock(activeLevel.features[y][x].block);
                if(activeLevel.terrain[y][x].block != FindBlock("air")->id){
                    PushRender_TilesheetEx(renderer, IDFindTilesheet(terrainBlock->tilesheet), terrainBlock->tileIndex, tile, activeLevel.terrain[y][x].zPos, activeLevel.terrain[y][x].rotation * 90, activeLevel.terrain[y][x].alpha, activeLevel.terrain[y][x].color);
                    activeLevel.terrain[y][x].zPos = RNDR_LEVEL + 1;
                    activeLevel.terrain[y][x].rotation = 0;
                    activeLevel.terrain[y][x].alpha = 255;
                    activeLevel.terrain[y][x].color = (SDL_Color){255, 255, 255};
                }
                if(activeLevel.features[y][x].block != FindBlock("air")->id){
                    PushRender_TilesheetEx(renderer, IDFindTilesheet(featureBlock->tilesheet), featureBlock->tileIndex, tile, activeLevel.features[y][x].zPos, activeLevel.features[y][x].rotation * 90, activeLevel.features[y][x].alpha, activeLevel.features[y][x].color);
                    activeLevel.features[y][x].zPos = RNDR_LEVEL + 1;
                    activeLevel.features[y][x].rotation = 0;
                    activeLevel.features[y][x].alpha = 255;
                    activeLevel.features[y][x].color = (SDL_Color){255, 255, 255};
                }
            }
        }
    }
}

void LoadLevel(char *path){

}

void SaveLevel(LevelObject *level, char *path){

}

// Level Interaction
BlockObject *PlaceBlock(TileObject **layer, BlockObject *block, Vector2 pos, int rotation){//Returns whether or not the block was placed
    // int layer = 0;
    // while(activeLevel.layer[layer][pos.y][pos.x].block = FindBlock("air")->id){
        
    // }
    if(pos.x > activeLevel.mapSize.x){
        pos.x = activeLevel.mapSize.x;
    }
    if(pos.y > activeLevel.mapSize.y){
        pos.y = activeLevel.mapSize.y;
    }
    int tmpBlock = layer[pos.y][pos.x].block;
    layer[pos.y][pos.x].block = block->id;
    layer[pos.y][pos.x].rotation = rotation;
    return IDFindBlock(tmpBlock);
}

BlockObject *RemoveBlock(TileObject **layer, Vector2 pos){
    int tmpBlock = layer[pos.y][pos.x].block;
    if(tmpBlock != undefinedBlock.id && tmpBlock != FindBlock("air")->id){
        layer[pos.y][pos.x].block = FindBlock("air")->id;
        return IDFindBlock(tmpBlock);
    }
    return &undefinedBlock;
}