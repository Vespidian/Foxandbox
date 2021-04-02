#include <unistd.h>

#include "../global.h"
#include "../utility.h"
#include "../render/renderer.h"
#include "../render/render_text.h"
#include "../event.h"
#include "chunk.h"
#include "sandbox_generation.h"

#include "sandbox.h"

int chunkLoadRadius = 2;

SandboxObject activeSandbox;
SandboxObject undefinedSandbox;

//Interaction functions
void RenderCursor();
void LevelMouseInteraction();
void LevelMovement();

//Mouse position
Vector2 mouseTilePos;
Vector2 mouseGlobalTilePos;

//Character position
Vector2 globalOffset = {0, 0};
Vector2 globalPosition = {0, 0};
// Vector2 globalCoordinates = {-52, -57};
Vector2 globalCoordinates = {0, 0};
// Vector2 globalCoordinates = {-20, -25};
// Vector2 globalCoordinates = {13, 8};

const char savePath[] = "saves/";

void InitSandboxes(){
    NewBlock(NewItem("air", FindTilesheet("default_tilesheet"), 0), FindItem("air"), FindTilesheet("default_tilesheet"), 0, false);
    
    NewEvent(EV_QUICK, SDL_MOUSEBUTTONDOWN, LevelMouseInteraction);
    NewEvent(EV_QUICK, SDL_KEYDOWN, LevelMovement);

    undefinedSandbox.isActive = false;
    activeSandbox = undefinedSandbox;


    DebugLog(D_ACT, "Initialized level subsystem");
}

void WritePlayerInfo(){
    char path[260];
    sprintf(path, "../%s%s/player.dat", savePath, activeSandbox.name);
    FILE *pInfo = fopen(path, "w");

    if(pInfo != NULL){
        fprintf(pInfo, \
            "coordx:%d\ncoordy:%d\noffsetx:%d\noffsety:%d\n",
            globalCoordinates.x,
            globalCoordinates.y,
            globalOffset.x,
            globalOffset.y
        );
        fclose(pInfo);
    }
}

void ReadPlayerInfo(){
    char path[260];
    sprintf(path, "../%s%s/player.dat", savePath, activeSandbox.name);
    FILE *pInfo = fopen(path, "r");

    if(pInfo != NULL){
        char *buffer;
        int length = GetLineLength(pInfo);

        buffer = malloc(sizeof(char) * length);
        while(fgets(buffer, length, pInfo)){

            char token[256];
            strcpy(token, strtok(buffer, ":"));
            if(strcmp(token, "coordx") == 0){
                globalCoordinates.x = strtol(strtok(NULL, "\n"), NULL, 10);
            }
            if(strcmp(token, "coordy") == 0){
                globalCoordinates.y = strtol(strtok(NULL, "\n"), NULL, 10);
            }
            if(strcmp(token, "offsetx") == 0){
                globalOffset.x = strtol(strtok(NULL, "\n"), NULL, 10);
            }
            if(strcmp(token, "offsety") == 0){
                globalOffset.y = strtol(strtok(NULL, "\n"), NULL, 10);
            }

            length = GetLineLength(pInfo);
            buffer = malloc(sizeof(char) * length);
        }
        fclose(pInfo);
        free(buffer);
    }else{
        // globalCoordinates.x = 0;
        // globalCoordinates.y = 0;
        // globalOffset.x = 0;
        // globalOffset.y = 0;
    }
}

void WriteSandboxInfo(){
    char path[260];
    sprintf(path, "../%s%s/sandbox.dat", savePath, activeSandbox.name);
    FILE *sInfo = fopen(path, "w");

    if(sInfo != NULL){
        fprintf(sInfo, "seed:%lu\n", activeSandbox.seed);
        fclose(sInfo);
    }
}

void ReadSandboxInfo(){
    
}

void WriteSandbox(char *name){
    /**
     ** Writes 'activeSandbox' to 'name' save folder **
     * 
     *  Write player.dat
     *  Write mods.conf
     *  Write all loaded chunks
     */
    if(activeSandbox.isActive){
        //Create folder shell
        char path[260];
        sprintf(path, "../%s%s/", savePath, name);
        _mkdir(path);
        sprintf(path, "../%s%s/chunks/", savePath, name);
        _mkdir(path);

        WritePlayerInfo();
        WriteSandboxInfo();
        for(int i = 0; i < activeSandbox.chunkBufferSize; i++){
            WriteChunk(&activeSandbox.chunkBuffer[i], activeSandbox.chunkBuffer[i].position);
        }
        DebugLog(D_VERBOSE_ACT, "Wrote sandbox '%s'", name);
    }else{
        DebugLog(D_WARN, "Tried to write to sandbox '%s' when no sandbox loaded", name);
    }
}

void ReadSandbox(char *name){
    /**
     *  Make sure sandbox name is different
     *  Unload current sandbox
     *  Read new sandbox into 'activeSandbox'
     *  Read player.dat
     *  Read mods.conf
     */
    // if()
    // UnloadSandbox();
    // NewSandbox(name, 334);
    // activeSandbox.id = 0;
    activeSandbox.name = malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(activeSandbox.name, name);
    activeSandbox.isActive = true;
    ReadPlayerInfo();

    //sets stuff like seed
    ReadSandboxInfo();
}

void UnloadSandbox(){
    /**
     *  Call WriteSandbox()
     *  Free chunkbuffer and set buffersize to 0
     */
    unsigned int start = SDL_GetTicks();
    if(activeSandbox.isActive){
        DebugLog(D_ACT, "Unloading sandbox with name '%s'", activeSandbox.name);
        WriteSandbox(activeSandbox.name);
        activeSandbox.chunkBufferSize = 0;
        free(activeSandbox.chunkBuffer);
        DebugLog(D_ACT, "Unloaded sandbox with name '%s', took %ums", activeSandbox.name, SDL_GetTicks() - start);
    }else{
        DebugLog(D_WARN, "Tried to unload sandbox when no sandbox loaded");
    }
    activeSandbox = undefinedSandbox;
}

void RenderSandbox(){
    Vector2 chunk = {0, 0};
    Vector2 chunkOffset = {0, 0};
    for(int y = 0; y <= chunkLoadRadius * 2 - 1; y++){
        for(int x = 0; x <= chunkLoadRadius * 2 - 1; x++){
            chunk = (Vector2){((globalCoordinates.x) / chunkSize) + x - 1, ((globalCoordinates.y) / chunkSize) + y - 1};
            chunkOffset.x = (x * chunkSize * tileRenderSize) + (-globalCoordinates.x % chunkSize) * tileRenderSize - globalOffset.x;
            chunkOffset.y = (y * chunkSize * tileRenderSize) + (-globalCoordinates.y % chunkSize) * tileRenderSize - globalOffset.y;
            
            chunkOffset.x += (SCREEN_WIDTH / 2) - (chunkSize * tileRenderSize * chunkLoadRadius) - tileRenderSize / 2;
            chunkOffset.y += (SCREEN_HEIGHT / 2) - (chunkSize * tileRenderSize * chunkLoadRadius) - tileRenderSize / 2;

            RenderChunk(FindChunk(chunk), chunkOffset);
        }
    }
    UnloadOldChunks();
    RenderCursor();
    CheckBufferGeneration();
}






BlockObject *PlaceBlock(BlockObject *block, Vector2 position, int layer, int rotation){//Returns whether or not the block was placed

    Vector2 chunkOffset = {position.x % chunkSize, position.y % chunkSize};
    Vector2 chunkCoord = {position.x / chunkSize, position.y / chunkSize};
    //Negative position
    if(position.x < 0){
        chunkOffset.x = (position.x + 1) % chunkSize + chunkSize - 1;
        chunkCoord.x = (position.x + 1) / chunkSize - 1;
    }
    if(position.y < 0){
        chunkOffset.y = (position.y + 1) % chunkSize + chunkSize - 1;
        chunkCoord.y = (position.y + 1) / chunkSize - 1;
    }
    int tmpBlock = FindChunk(chunkCoord)->tile[layer][chunkOffset.y][chunkOffset.x].block;
    FindChunk(chunkCoord)->tile[layer][chunkOffset.y][chunkOffset.x].block = block->id;
    FindChunk(chunkCoord)->tile[layer][chunkOffset.y][chunkOffset.x].rotation = rotation;
    return IDFindBlock(tmpBlock);
}

void RenderCursor(){
    Vector2 chunkCenterOffset = {
        (SCREEN_WIDTH / 2) - (chunkSize * tileRenderSize * chunkLoadRadius) - tileRenderSize / 2,
        (SCREEN_HEIGHT / 2) - (chunkSize * tileRenderSize * chunkLoadRadius) - tileRenderSize / 2
    };

    mouseTilePos.x = (globalOffset.x + mousePos.x - chunkCenterOffset.x % tileRenderSize) / tileRenderSize;
    mouseTilePos.y = (globalOffset.y + mousePos.y - chunkCenterOffset.y % tileRenderSize) / tileRenderSize;
    SDL_Rect cursor = {
        mouseTilePos.x * tileRenderSize + chunkCenterOffset.x % tileRenderSize - globalOffset.x, 
        mouseTilePos.y * tileRenderSize + chunkCenterOffset.y % tileRenderSize - globalOffset.y, 
        tileRenderSize, 
        tileRenderSize
    };

    PushRender_Tilesheet(renderer, FindTilesheet("builtin"), 2, cursor, RNDR_UI);

    mouseGlobalTilePos = (Vector2){
        mouseTilePos.x + globalCoordinates.x - chunkCenterOffset.x / tileRenderSize - chunkSize, 
        mouseTilePos.y + globalCoordinates.y - chunkCenterOffset.y / tileRenderSize - chunkSize
    };
    
    Vector2 infoGroup = {0, 0};
    RenderText(renderer, FindFont("default_font"), 1, infoGroup.x, infoGroup.y, "mouseTilePos: %d, %d", mouseTilePos.x, mouseTilePos.y);
    RenderText(renderer, FindFont("default_font"), 1, infoGroup.x, infoGroup.y + 20, "mouseGlobalTilePos: %d, %d", mouseGlobalTilePos.x, mouseGlobalTilePos.y);
    RenderText(renderer, FindFont("default_font"), 1, infoGroup.x, infoGroup.y + 40, "globalOffset: %d, %d", globalOffset.x, globalOffset.y);
    RenderText(renderer, FindFont("default_font"), 1, infoGroup.x, infoGroup.y + 60, "globalCoordinates: %d, %d", globalCoordinates.x, globalCoordinates.y);
    Vector2 chunkOut = {0, 0};
    Vector2 chunkOffset = {0, 0};
    CoordinateConvert(mouseGlobalTilePos, &chunkOut, &chunkOffset);
    RenderText(renderer, FindFont("default_font"), 1, infoGroup.x, infoGroup.y + 80, "chunk: %d, %d", chunkOut.x, chunkOut.y);
}

void LevelMouseInteraction(EventData event){
    if(mouseHeld && activeSandbox.isActive){
        if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)){
            PlaceBlock(FindBlock("water"), mouseGlobalTilePos, 0, 0);
        }
        if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)){
            PlaceBlock(FindBlock("air"), mouseGlobalTilePos, 0, 0);
        }
	}
}

void LevelMovement(EventData event){
    // int x = 0;
    // int y = 0;
    int speed = 3;
    if(event.keyStates[SDL_SCANCODE_W]){
		globalPosition.y -= speed * deltatime;
        // y = -1;
	}
	if(event.keyStates[SDL_SCANCODE_S]){
		globalPosition.y += speed * deltatime;
        // y = 1;
	}
	if(event.keyStates[SDL_SCANCODE_A]){
		globalPosition.x -= speed * deltatime;
        // x = -1;
	}
	if(event.keyStates[SDL_SCANCODE_D]){
		globalPosition.x += speed * deltatime;
        // x = 1;
	}

    // if(abs((int)globalOffset.x) + 1 > tileRenderSize / 2){
    //     globalCoordinates.x += (globalOffset.x / (tileRenderSize / 2));
    //     // globalOffset.x = (-((int)globalOffset.x % tileRenderSize) + x) % (tileRenderSize / 2);
    //     globalOffset.x = (-(((int)globalOffset.x >> 5) & 1) + x) % (tileRenderSize / 2);
    // }
    // if(abs((int)globalOffset.y) + 1 > tileRenderSize / 2){
    //     globalCoordinates.y += (globalOffset.y / (tileRenderSize / 2));
    //     globalOffset.y = (-((int)globalOffset.y % tileRenderSize) + y) % (tileRenderSize / 2);
    // }
    globalOffset.x = globalPosition.x % tileRenderSize;
    globalOffset.y = globalPosition.y % tileRenderSize;
    globalCoordinates.y = globalPosition.y / tileRenderSize;
    globalCoordinates.x = globalPosition.x / tileRenderSize;
}