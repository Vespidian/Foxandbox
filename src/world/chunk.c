#include "../global.h"
#include "block.h"
#include "../render/renderer.h"
#include "../utility.h"
#include "sandbox.h"
#include "sandbox_generation.h"
#include "../ui/resizable_rect.h"

#include "chunk.h"

const int chunkSize = 32;
const int chunkLayers = 4;

int tileRenderSize = 64;
int chunkTimeoutLimit = 5000;

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
    chunk->lastAccess = SDL_GetTicks();
    chunk->isGenerated = false;
    activeSandbox.chunkBufferSize++;


    // RandomFill(position, 50);
	// IterateCellularAutomata(chunk);
	// IterateCellularAutomata(chunk->position);
    // FillChunk(position);
    return chunk;
}

void UnloadChunk(Vector2 coordinate){
    int offset = -1;
    for(int i = 0; i < activeSandbox.chunkBufferSize; i++){
        if(CompareVector2(activeSandbox.chunkBuffer[i].position, coordinate)){
            offset = i;
        }
    }
    if(offset > -1){
        for(int j = offset; j < activeSandbox.chunkBufferSize - 1; j++){
            activeSandbox.chunkBuffer[j] = activeSandbox.chunkBuffer[j + 1];
        }
    }
    activeSandbox.chunkBufferSize--;
    activeSandbox.chunkBuffer = realloc(activeSandbox.chunkBuffer, sizeof(ChunkObject) * activeSandbox.chunkBufferSize);
}

void UnloadOldChunks(){
    for(int i = 0; i < activeSandbox.chunkBufferSize; i++){
        if(SDL_GetTicks() - activeSandbox.chunkBuffer[i].lastAccess >= chunkTimeoutLimit){
            UnloadChunk(activeSandbox.chunkBuffer[i].position);
        }
    }
}

void WriteChunk(ChunkObject *chunk, Vector2 position){
    char name[260];
    snprintf(name, 260, "../%s%s/chunks/%d,%d", savePath, activeSandbox.name, position.x, position.y);
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
    DebugLog(D_VERBOSE_ACT, "Wrote to chunk '%d,%d' in sandbox '%s'", position.x, position.y, name);
}

ChunkObject *ReadChunk(Vector2 position){
    char name[260];
    snprintf(name, 260, "../%s%s/chunks/%d,%d", savePath, activeSandbox.name, position.x, position.y);
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
    chunk->lastAccess = SDL_GetTicks();
    DebugLog(D_VERBOSE_ACT, "Read from chunk '%d,%d' in sandbox '%s'", position.x, position.y, name);
    return chunk;
}

ChunkObject *FindChunk(Vector2 coordinate){
    //Check chunk buffer
    for(int i = 0; i < activeSandbox.chunkBufferSize; i++){
        if(CompareVector2(activeSandbox.chunkBuffer[i].position, coordinate)){
            activeSandbox.chunkBuffer[i].lastAccess = SDL_GetTicks();
            return &activeSandbox.chunkBuffer[i];
        }
    }
    //Check sandbox folder
    return ReadChunk(coordinate);
}

bool CheckChunkExists(Vector2 chunk){
    for(int i = 0; i < activeSandbox.chunkBufferSize; i++){
        if(CompareVector2(activeSandbox.chunkBuffer[i].position, chunk)){
            return true;
        }
    }
    char chunk_name[32];
    snprintf(chunk_name, 32, "%d %d", chunk.x, chunk.y);
    FILE *chunk_file;
    if((chunk_file = fopen(chunk_name, "r")) != NULL){
        fclose(chunk_file);
        return true;
    }

    return false;
}

void RenderChunk(ChunkObject *chunk, Vector2 position){
    SDL_Rect chunkRect = {position.x, position.y, chunkSize * tileRenderSize, chunkSize * tileRenderSize};

    if(SDL_HasIntersection(GetWindowRect(window), &chunkRect)){
        PushRender_Tilesheet(renderer, FindTilesheet("builtin"), 3, chunkRect, RNDR_LEVEL - 1);
        ResizableRect(chunkRect, 5);
        SDL_Rect tileRect = {0, 0, tileRenderSize, tileRenderSize};
        for(int y = (-position.y / tileRenderSize) * (position.y < 0); y < ((-position.y + SCREEN_HEIGHT) / tileRenderSize + 1) && y < chunkSize; y++){
            for(int x = (-position.x / tileRenderSize) * (position.x < 0); x < ((-position.x + SCREEN_WIDTH) / tileRenderSize + 1) && x < chunkSize; x++){
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
        chunk->lastAccess = SDL_GetTicks();
    }
}