#include <math.h>

#include "../global.h"
#include "block.h"
#include "../render/renderer.h"
#include "../event.h"
#include "../utility.h"
#include "level.h"


#include "../render/render_text.h"

const int MAXLEVELSIZE = 512;
const int chunkSize = 4;
const int chunkLayers = 2;

int chunkLoadRadius = 1;
int tileRenderSize = 64;

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

    activeSandbox.path = malloc(sizeof(char) * 256);
    strcpy(activeSandbox.path, "../saves/test_world1");
}

ChunkObject *NewChunk(Vector2 position){
    activeSandbox.chunkBuffer = realloc(activeSandbox.chunkBuffer, sizeof(ChunkObject) * (activeSandbox.chunkBufferSize + 1));
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
    chunk->position = position;
    activeSandbox.chunkBufferSize++;
    // FillChunk(position);
    return chunk;
}

void WriteChunk(ChunkObject *chunk, Vector2 position){
    char name[260];
    snprintf(name, 260, "%s/sandbox/%d,%d", activeSandbox.path, position.x, position.y);
    // FILE *chunkFile = fopen(name, "w");

}

ChunkObject *ReadChunk(Vector2 position){
    char name[260];
    snprintf(name, 260, "%s/sandbox/%d,%d", activeSandbox.path, position.x, position.y);
    FILE *chunkFile = fopen(name, "r");
    ChunkObject *chunk = NewChunk(position);

    if(chunkFile != NULL){
        char header[64];
        char *lineBuffer;
        int lineLength = GetLineLength(chunkFile);
        int line = 0;

        int layerY = 0;
        int chunkLayer = 0;

        lineBuffer = malloc(sizeof(char) * lineLength);
        while(fgets(lineBuffer, lineLength, chunkFile)){
            if(lineBuffer[0] == ':' && lineBuffer[1] == ':'){// Check for heading
                strshft_l(lineBuffer, 0, 2);
                strcpy(header, lineBuffer);

            }
            if(strcmp(header, "layers") == 0){// Reading chunk layers
                char *subhead = strchr(lineBuffer, '-');// Check for subheading (in this case which layer we are reading)
                
                if(*subhead == '-' && *(subhead + 1) == '>'){
                    chunkLayer = *(subhead + 2) - '0';
                    layerY = 0;

                }else{
                    char tile[128];
                    strcpy(tile, strtok(lineBuffer, ","));
                    for(int x = 0; x < chunkSize; x++){// Iterate through line to read 'chunkSize' tiles
                        int rotation = 0;

                        if(strchr(tile, '~') != NULL){// Check if tile has rotation
                            rotation = strtol(strchr(tile, '~'), NULL, 10);// Extract tile rotation
                            *(strchr(tile, '~')) = '\0';//Leave behind only the tile name
                        }

                        chunk->tile[chunkLayer][layerY][x].rotation = rotation;// Assign rotation value
                        chunk->tile[chunkLayer][layerY][x].block = FindBlock(tile)->id;// Assign block based on tile name

                        char *tmp = strtok(NULL, ",");// Read next tile
                        if(tmp != NULL){// Make sure tile is read correctly
                            strcpy(tile, tmp);
                        }else{// Otherwise set it to undefined
                            strcpy(tile, "undefined");
                        }

                    }
                    layerY++;// Increment layer 'layerY' position for next line read
                }
            }
            if(strcmp(header, "entities") == 0){}

            line++;
        }
        free(lineBuffer);
        fclose(chunkFile);
    }else{
        WriteChunk(chunk, position);
    }
    return chunk;
}

ChunkObject *FindChunk(Vector2 coordinate){
    //Check chunk buffer
    for(int i = 0; i < activeSandbox.chunkBufferSize; i++){
        if(CompareVector2(activeSandbox.chunkBuffer[i].position, coordinate)){
            return &activeSandbox.chunkBuffer[i];
        }
    }
    //Check currently rendered chunks
    // for(int y = 0; y < 2; y++){
    //     for(int x = 0; x < 2; x++){
    //         if(CompareVector2(activeSandbox.chunk[y][x].position, coordinate)){
    //             return &activeSandbox.chunk[y][x];
    //         }
    //     }
    // }
    //Check sandbox folder
    return ReadChunk(coordinate);
}

void RenderChunk(ChunkObject *chunk, Vector2 position){
    SDL_Rect chunkRect = {-position.x, -position.y, chunkSize * tileRenderSize, chunkSize * tileRenderSize};

    if(SDL_HasIntersection(GetWindowRect(window), &chunkRect)){
        PushRender_Tilesheet(renderer, FindTilesheet("builtin"), 3, chunkRect, RNDR_LEVEL - 1);
        SDL_Rect tileRect = {0, 0, tileRenderSize, tileRenderSize};
        for(int y = (chunkSize - (position.y / tileRenderSize)) * ((position.y / tileRenderSize - 1) > 0); y * tileRenderSize + position.y < SCREEN_HEIGHT && y < chunkSize; y++){
            for(int x = (chunkSize - (position.x / tileRenderSize)) * ((position.x / tileRenderSize - 1) > 0); x * tileRenderSize + position.x < SCREEN_WIDTH && x < chunkSize; x++){
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
                        chunk->tile[z][y][x].zPos = RNDR_LEVEL + z + 1;
                        chunk->tile[z][y][x].rotation = 0;
                        chunk->tile[z][y][x].alpha = 255;
                        chunk->tile[z][y][x].color = (SDL_Color){255, 255, 255};
                    }
                }
            }
        }
    }
}

void RenderSandbox(){
    for(int y = -chunkLoadRadius; y <= chunkLoadRadius; y++){
        for(int x = -chunkLoadRadius; x <= chunkLoadRadius; x++){
            RenderChunk(
                FindChunk((Vector2){(-globalCoordinates.x / chunkSize) + x, (-globalCoordinates.y / chunkSize) + y}), 
                (Vector2){//THIS MUST BE FIXED / CLEANED UP
                    (-(chunkLoadRadius * chunkSize * tileRenderSize) + (x * chunkSize * tileRenderSize) + (globalCoordinates.x % chunkSize) * tileRenderSize) + globalOffset.x - (SCREEN_WIDTH / 2) + (((chunkLoadRadius * 2 + 1) * chunkSize * tileRenderSize) / 2),
                    (-(chunkLoadRadius * chunkSize * tileRenderSize) + (y * chunkSize * tileRenderSize) + (globalCoordinates.y % chunkSize) * tileRenderSize) + globalOffset.y - (SCREEN_HEIGHT / 2) + (((chunkLoadRadius * 2 + 1) * chunkSize * tileRenderSize) / 2)
                }
            );
        }
    }

    // RenderText(renderer, FindFont("default"), 1, 100, 100, "%d, %d", (globalCoordinates.x / chunkSize), (globalCoordinates.y / chunkSize));
    // RenderText(renderer, FindFont("default"), 1, 100, 120, "%f, %f", globalOffset.x, globalOffset.y);
    // RenderText(renderer, FindFont("default"), 1, 100, 140, "%d, %d", globalCoordinates.x, globalCoordinates.y);
    // RenderCursor();
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

void FillChunk(Vector2 coordinate){
    for(int y = 0; y < chunkSize; y++){
        for(int x = 0; x < chunkSize; x++){
            FindChunk(coordinate)->tile[0][y][x].block = FindBlock("grass")->id;
        }
    }
}

void LevelMouseInteraction(EventData event){
    if(mouseHeld){
        // PlaceBlock(activeLevel.terrain, FindBlock("water"), mouseTilePos, 0);
	}
}

void LevelMovement(EventData event){
    int x = 0;
    int y = 0;
    if(event.keyStates[SDL_SCANCODE_W]){
		globalOffset.y -= 1 * deltatime;
        y = -1;
	}
	if(event.keyStates[SDL_SCANCODE_S]){
		globalOffset.y += 1 * deltatime;
        y = 1;
	}
	if(event.keyStates[SDL_SCANCODE_A]){
		globalOffset.x -= 1 * deltatime;
        x = -1;
	}
	if(event.keyStates[SDL_SCANCODE_D]){
		globalOffset.x += 1 * deltatime;
        x = 1;
	}

    if(abs((int)globalOffset.x) + 1 > tileRenderSize / 2){
        globalCoordinates.x += ((globalOffset.x) < 0) ? -1 : 1;
        globalOffset.x = -fmodf(globalOffset.x, tileRenderSize) + x;
    }
    if(abs((int)globalOffset.y) + 1 > tileRenderSize / 2){
        globalCoordinates.y += ((globalOffset.y) < 0) ? -1 : 1;
        globalOffset.y = -fmodf(globalOffset.y, tileRenderSize) + y;
    }
}

void KeyUp(EventData event){
    // Vector2 pixelPerfectOffset = {(int)globalOffset.x % 4, (int)globalOffset.y % 4};
    // if(pixelPerfectOffset.x != 0){
    //     globalOffset.x += pixelPerfectOffset.x >= 3 ? pixelPerfectOffset.x : -(pixelPerfectOffset.x);
    // }
    // if(pixelPerfectOffset.y != 0){
    //     globalOffset.y += pixelPerfectOffset.y >= 3 ? pixelPerfectOffset.y : -(pixelPerfectOffset.y);
    // }
}