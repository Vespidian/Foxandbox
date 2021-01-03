#include "../global.h"
#include "block.h"
#include "../render/renderer.h"
#include "../event.h"
#include "../utility.h"
#include "level.h"

const int MAXLEVELSIZE = 512;
int tileRenderSize = 64;
const int chunkSize = 4;
const int chunkLayers = 2;

// LevelObject *levels;
// unsigned int numLevels = 0;
// static unsigned int nextID = 0;
// LevelObject activeLevel;
Vector2 mouseTilePos;


fVector2 globalOffset = {0, 0};
Vector2 globalCoordinates = {0, 0};

SandboxObject *sandboxes;
SandboxObject activeSandbox;


void RenderCursor();
void LevelMouseInteraction();
void LevelMovement();
void KeyUp(EventData event);


void InitLevels(){
    NewBlock(NewItem("air", FindTilesheet("default_tilesheet"), 0), FindItem("air"), FindTilesheet("default_tilesheet"), 0, false);
    
    NewEvent(EV_QUICK, SDL_MOUSEBUTTONDOWN, LevelMouseInteraction);
    NewEvent(EV_QUICK, SDL_KEYDOWN, LevelMovement);
    NewEvent(EV_ACCURATE, SDL_KEYUP, KeyUp);
    
    DebugLog(D_ACT, "Initialized level subsystem");
}


void RenderSandbox(){

    // RenderCursor();
}

ChunkObject *FindChunk(Vector2 coordinate){
    return NULL;
}

void RenderChunk(ChunkObject *chunk, Vector2 position){
    SDL_Rect chunkRect = {-position.x, -position.y, chunkSize * tileRenderSize, chunkSize * tileRenderSize};

    if(SDL_HasIntersection(GetWindowRect(window), &chunkRect)){
        SDL_Rect tileRect = {0, 0, tileRenderSize, tileRenderSize};

        for(int y = (position.y / -tileRenderSize - 1) * (position.y / -tileRenderSize - 1 > 0);  y < ((position.y + SCREEN_HEIGHT) / tileRenderSize + 1) && y < chunkSize; y++){
            for(int x = (position.x / -tileRenderSize - 1) * (position.x / -tileRenderSize - 1 > 0);  x < ((position.x + SCREEN_HEIGHT) / tileRenderSize + 1) && x < chunkSize; x++){
                tileRect.x = (x * tileRenderSize) - position.x;
                tileRect.y = (y * tileRenderSize) - position.y;
                for(int z = 0; z < chunkLayers; z++){
                    BlockObject *terrainBlock = IDFindBlock(chunk->tile[z][y][x].block);
                    if(chunk->tile[z][y][x].block != FindBlock("air")->id){
                        PushRender_TilesheetEx(
                            renderer, 
                            IDFindTilesheet(terrainBlock->tilesheet), 
                            terrainBlock->tileIndex, 
                            tileRect, 
                            chunk->tile[z][y][x].zPos, 
                            chunk->tile[z][y][x].rotation * 90, 
                            chunk->tile[z][y][x].alpha, 
                            chunk->tile[z][y][x].color
                        );
                        chunk->tile[z][y][x].zPos = RNDR_LEVEL + 1;
                        chunk->tile[z][y][x].rotation = 0;
                        chunk->tile[z][y][x].alpha = 255;
                        chunk->tile[z][y][x].color = (SDL_Color){255, 255, 255};
                    }
                }
            }
        }
    }
}

ChunkObject *NewChunk(Vector2 position){
    activeSandbox.chunkBuffer = realloc(activeSandbox.chunkBuffer, sizeof(ChunkObject) * activeSandbox.chunkBufferSize);
    ChunkObject *chunk = &activeSandbox.chunkBuffer[activeSandbox.chunkBufferSize];
    
    TileObject templateTile = {FindBlock("air")->id, 0, 0, 255, (SDL_Color){255, 255, 255}};
    for(int y = 0; y < chunkSize; y++){
        memset(chunk->collision[y], -1, sizeof(int) * (chunkSize));
        for(int x = 0; x < chunkSize; x++){
            for(int z = 0; z < chunkLayers; z++){
                chunk->tile[z][y][x] = (TileObject)templateTile;
            }
        }
    }
    activeSandbox.chunkBufferSize++;
    return chunk;
}

void WriteChunk(ChunkObject *chunk, Vector2 position){
    char name[48];
    snprintf(name, 48, "%d,%d", position.x, position.y);
    // FILE *chunkFile = fopen(name, "w");

}

ChunkObject *ReadChunk(Vector2 position){
    char name[48];
    snprintf(name, 48, "%d,%d", position.x, position.y);
    FILE *chunkFile = fopen(name, "r");
    ChunkObject *chunk = NewChunk(position);

    if(chunkFile != NULL){
        
    }else{
        WriteChunk(chunk, position);
    }


    return chunk;
}



// Level Interaction
/*BlockObject *PlaceBlock(TileObject **layer, BlockObject *block, Vector2 pos, int rotation){//Returns whether or not the block was placed
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
}*/

/*void RenderCursor(){
    SDL_Rect levelRect = {-globalOffset.x, -globalOffset.y, activeLevel.mapSize.x * tileRenderSize, activeLevel.mapSize.y * tileRenderSize};
    if(SDL_PointInRect((SDL_Point*)&mousePos, &levelRect)){
        mouseTilePos.x = (mousePos.x + globalOffset.x) / tileRenderSize;
        mouseTilePos.y = (mousePos.y + globalOffset.y) / tileRenderSize;
        SDL_Rect cursor = {(mouseTilePos.x * 64) - globalOffset.x, (mouseTilePos.y * 64) - globalOffset.y, tileRenderSize, tileRenderSize};
        PushRender_Tilesheet(renderer, FindTilesheet("builtin"), 2, cursor, RNDR_UI);
    }
}*/

void LevelMouseInteraction(EventData event){
    if(mouseHeld){
        // PlaceBlock(activeLevel.terrain, FindBlock("water"), mouseTilePos, 0);
	}
}

void LevelMovement(EventData event){
    if(event.keyStates[SDL_SCANCODE_W]){
		globalOffset.y -= 1 * deltatime;
	}
	if(event.keyStates[SDL_SCANCODE_S]){
		globalOffset.y += 1 * deltatime;
	}
	if(event.keyStates[SDL_SCANCODE_A]){
		globalOffset.x -= 1 * deltatime;
	}
	if(event.keyStates[SDL_SCANCODE_D]){
		globalOffset.x += 1 * deltatime;
	}

    if(abs((int)globalOffset.x % tileRenderSize) > tileRenderSize / 2){
        globalCoordinates.x += ((globalOffset.x) < 0) ? -1 : 1;
        globalOffset.x = (int)globalOffset.x % (tileRenderSize / 2);
    }
    if(abs((int)globalOffset.y % tileRenderSize) > tileRenderSize / 2){
        globalCoordinates.y += ((globalOffset.y) < 0) ? -1 : 1;
        globalOffset.y = (int)globalOffset.y % (tileRenderSize / 2);
    }
}

void KeyUp(EventData event){
    Vector2 pixelPerfectOffset = {(int)globalOffset.x % 4, (int)globalOffset.y % 4};
    if(pixelPerfectOffset.x != 0){
        globalOffset.x += pixelPerfectOffset.x >= 3 ? pixelPerfectOffset.x : -(pixelPerfectOffset.x);
    }
    if(pixelPerfectOffset.y != 0){
        globalOffset.y += pixelPerfectOffset.y >= 3 ? pixelPerfectOffset.y : -(pixelPerfectOffset.y);
    }
}