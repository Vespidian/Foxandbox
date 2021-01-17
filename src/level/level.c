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
const int chunkLayers = 4;

int chunkLoadRadius = 1;
int tileRenderSize = 64;

// LevelObject *levels;
// unsigned int numLevels = 0;
// static unsigned int nextID = 0;
// LevelObject activeLevel;
Vector2 mouseTilePos;
Vector2 mouseGlobalTilePos;


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
    FILE *chunkFile = fopen(name, "w");

    fprintf(chunkFile, "::layers\n");
    for(int z = 0; z < chunkLayers; z++){
        fprintf(chunkFile, "->%d\n", z);
        for(int y = 0; y < chunkSize; y++){
            for(int x = 0; x < chunkSize; x++){
                fprintf(chunkFile, "%s,", IDFindItem(IDFindBlock(chunk->tile[z][y][x].block)->item)->name);
                if(chunk->tile[z][y][x].rotation > 0){
                    fprintf(chunkFile, "~%d", chunk->tile[z][y][x].rotation);
                }
            }
            fprintf(chunkFile, "\n");
        }
    }
    fprintf(chunkFile, "::entities\n");
    fprintf(chunkFile, "::drops\n");
    fclose(chunkFile);
}

ChunkObject *ReadChunk(Vector2 position){
    char name[260];
    snprintf(name, 260, "%s/sandbox/%d,%d", activeSandbox.path, position.x, position.y);
    FILE *chunkFile = fopen(name, "r");
    ChunkObject *chunk = NewChunk(position);

    if(chunkFile != NULL){
        char header[128];
        char *lineBuffer;
        int lineLength = GetLineLength(chunkFile);
        int line = 0;

        int layerY = 0;
        int chunkLayer = -1;

        lineBuffer = malloc(sizeof(char) * lineLength);
        while(fgets(lineBuffer, lineLength, chunkFile)){
            if(strcmp(header, "layers") == 0){// Reading chunk layers
                if(lineBuffer[0] == '-' && lineBuffer[1] == '>'){
                    chunkLayer++;// Convert character to int
                    layerY = 0;// Reset y value
                }else if(layerY < chunkSize){
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
            // if(strcmp(header, "entities") == 0){}
            // if(strcmp(header, "drops") == 0){}

            if(lineBuffer[0] == ':' && lineBuffer[1] == ':'){// Check for heading
                strshft_l(lineBuffer, 0, 2);// Remove '::' identifiers
                strcpy(header, lineBuffer);
                header[strlen(header) - 1] = '\0';
            }
            lineLength = GetLineLength(chunkFile);
            lineBuffer = malloc(sizeof(char) * lineLength);
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
    //Check sandbox folder
    return ReadChunk(coordinate);
}

void RenderChunk(ChunkObject *chunk, Vector2 position){
    SDL_Rect chunkRect = {position.x, position.y, chunkSize * tileRenderSize, chunkSize * tileRenderSize};

    if(SDL_HasIntersection(GetWindowRect(window), &chunkRect)){
        PushRender_Tilesheet(renderer, FindTilesheet("builtin"), 3, chunkRect, RNDR_LEVEL - 1);
        SDL_Rect tileRect = {0, 0, tileRenderSize, tileRenderSize};
        // for(int y = (chunkSize - (position.y / tileRenderSize)) * ((position.y / tileRenderSize - 1) > 0); position.y + (y * tileRenderSize) < SCREEN_HEIGHT && y < chunkSize; y++){
            // for(int x = (chunkSize - (position.x / tileRenderSize)) * ((position.x / tileRenderSize - 1) > 0); position.x + (x * tileRenderSize) < SCREEN_WIDTH && x < chunkSize; x++){
        for(int y = 0; y < chunkSize; y++){
            for(int x = 0; x < chunkSize; x++){
                tileRect.x = (x * tileRenderSize) + position.x;
                tileRect.y = (y * tileRenderSize) + position.y;
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
    Vector2 chunkP = {0, 0};
    for(int y = 0; y <= chunkLoadRadius * 2; y++){
        for(int x = 0; x <= chunkLoadRadius * 2; x++){
            chunkP = (Vector2){((globalCoordinates.x) / chunkSize) + x - 1, ((globalCoordinates.y) / chunkSize) + y - 1};
            // RenderText(renderer, FindFont("default_font"), 1, (globalCoordinates.x % chunkSize) - (globalOffset.x) + x * chunkSize * tileRenderSize + 16, (globalCoordinates.y % chunkSize) - (globalOffset.y) + y * chunkSize * tileRenderSize + 16, "%d, %d", chunkP.x, chunkP.y);
            RenderChunk(
                FindChunk(chunkP), 
                (Vector2){
                    (x * chunkSize * tileRenderSize) + (-globalCoordinates.x % chunkSize) * tileRenderSize - globalOffset.x - (chunkSize * tileRenderSize),
                    (y * chunkSize * tileRenderSize) + (-globalCoordinates.y % chunkSize) * tileRenderSize - globalOffset.y - (chunkSize * tileRenderSize)
                }
            );
        }
    }

    // RenderText(renderer, FindFont("default"), 1, 100, 100, "%d, %d", (globalCoordinates.x / chunkSize), (globalCoordinates.y / chunkSize));
    // RenderText(renderer, FindFont("default"), 1, 100, 120, "%f, %f", globalOffset.x, globalOffset.y);
    // RenderText(renderer, FindFont("default"), 1, 100, 140, "%d, %d", globalCoordinates.x, globalCoordinates.y);
    RenderCursor();
}

void LoadWorld(char *path){

}

void UnloadWorld(){
    for(int i = 0; i < activeSandbox.chunkBufferSize; i++){
        WriteChunk(&activeSandbox.chunkBuffer[i], activeSandbox.chunkBuffer[i].position);
        // printf("wrote chunk %d, %d\n", activeSandbox.chunkBuffer[i].position.x, activeSandbox.chunkBuffer[i].position.y);
    }
}

void FillChunk(Vector2 coordinate){
    for(int y = 0; y < chunkSize; y++){
        for(int x = 0; x < chunkSize; x++){
            FindChunk(coordinate)->tile[0][y][x].block = FindBlock("grass")->id;
        }
    }
}

// Level Interaction
Vector2 chunkOffset;
Vector2 chunkCoord;
BlockObject *PlaceBlock(BlockObject *block, Vector2 position, int layer, int rotation){//Returns whether or not the block was placed

    chunkOffset = (Vector2){position.x % chunkSize, position.y % chunkSize};
    chunkCoord = (Vector2){position.x / chunkSize, position.y / chunkSize};
    //Negative position
    if(position.x < 0){
        chunkOffset.x = (position.x + 1) % chunkSize + chunkSize - 1;
        chunkCoord.x = (position.x + 1) / chunkSize - 1;
    }
    if(position.y < 0){
        chunkOffset.y = (position.y + 1) % chunkSize + chunkSize - 1;
        chunkCoord.y = (position.y + 1) / chunkSize - 1;
    }
    chunkCoord.x++;
    chunkCoord.y++;
    int tmpBlock = FindChunk(chunkCoord)->tile[layer][chunkOffset.y][chunkOffset.x].block;
    FindChunk(chunkCoord)->tile[layer][chunkOffset.y][chunkOffset.x].block = block->id;
    FindChunk(chunkCoord)->tile[layer][chunkOffset.y][chunkOffset.x].rotation = rotation;
    // printf("placed block in chunk: %d, %d offset: %d, %d\n global coordinate: %d, %d\n\n", 
    //     chunkCoord.x,
    //     chunkCoord.y,
    //     chunkOffset.x,
    //     chunkOffset.y,
    //     position.x,
    //     position.y
    // );
    return IDFindBlock(tmpBlock);
}

/*BlockObject *RemoveBlock(TileObject **layer, Vector2 pos){
    int tmpBlock = layer[pos.y][pos.x].block;
    if(tmpBlock != undefinedBlock.id && tmpBlock != FindBlock("air")->id){
        layer[pos.y][pos.x].block = FindBlock("air")->id;
        return IDFindBlock(tmpBlock);
    }
    return &undefinedBlock;
}*/

void RenderCursor(){
    mouseTilePos.x = (globalOffset.x + mousePos.x) / tileRenderSize;
    mouseTilePos.y = (globalOffset.y + mousePos.y) / tileRenderSize;
    SDL_Rect cursor = {(mouseTilePos.x * tileRenderSize) - globalOffset.x, mouseTilePos.y * tileRenderSize - globalOffset.y, tileRenderSize, tileRenderSize};
    PushRender_Tilesheet(renderer, FindTilesheet("builtin"), 2, cursor, RNDR_UI);

    mouseGlobalTilePos = (Vector2){mouseTilePos.x + globalCoordinates.x - chunkSize, mouseTilePos.y + globalCoordinates.y - chunkSize};
    Vector2 infoGroup = {100, 0};
    RenderText(renderer, FindFont("default_font"), 1, infoGroup.x, infoGroup.y, "mouseTilePos: %d, %d", mouseTilePos.x, mouseTilePos.y);
    RenderText(renderer, FindFont("default_font"), 1, infoGroup.x, infoGroup.y + 20, "mouseGlobalTilePos: %d, %d", mouseGlobalTilePos.x, mouseGlobalTilePos.y);
    RenderText(renderer, FindFont("default_font"), 1, infoGroup.x, infoGroup.y + 40, "chunkOffset: %d, %d", chunkOffset.x, chunkOffset.y);
    RenderText(renderer, FindFont("default_font"), 1, infoGroup.x, infoGroup.y + 60, "chunkCoord: %d, %d", chunkCoord.x, chunkCoord.y);
}

void LevelMouseInteraction(EventData event){
    if(mouseHeld){
        if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)){
            PlaceBlock(FindBlock("water"), mouseGlobalTilePos, 0, 0);
        }
        if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)){
            PlaceBlock(FindBlock("air"), mouseGlobalTilePos, 0, 0);
        }
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