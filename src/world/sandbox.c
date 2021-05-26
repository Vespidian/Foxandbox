#include <unistd.h>

#include "../global.h"
#include "../utility.h"
// #include "../renderer/renderer.h"
#include "../renderer/quad.h"
#include "../renderer/render_text.h"
#include "../event.h"
#include "chunk.h"
#include "sandbox_generation.h"

#include "sandbox.h"

int chunk_load_radius = 2;

SandboxObject active_sandbox;
SandboxObject undefined_sandbox;

//Interaction functions
void RenderCursor();
void LevelMouseInteraction();
void LevelMovement();

//Mouse position
iVector2 mouse_tile_pos;
iVector2 mouse_global_tile_pos;

//Character position
iVector2 player_coordinate_offset = {0, 0};
iVector2 global_position = {0, 0};
// iVector2 player_coordinate = {-52, -57};
iVector2 player_coordinate = {0, 0};
// iVector2 player_coordinate = {-20, -25};
// iVector2 player_coordinate = {13, 8};

const char save_path[] = "../saves/";

void InitSandboxes(){
    // NewBlock(NewItem("air", FindTilesheet("default_tilesheet"), 0), FindItem("air"), FindTilesheet("default_tilesheet"), 0, false);
    
    BindEvent(EV_QUICK, SDL_MOUSEBUTTONDOWN, LevelMouseInteraction);
    BindEvent(EV_QUICK, SDL_KEYDOWN, LevelMovement);

    undefined_sandbox.isActive = false;
    active_sandbox = undefined_sandbox;


    DebugLog(D_ACT, "Initialized level subsystem");
}

void WritePlayerInfo(){
    char path[260];
    sprintf(path, "%s%s/player.dat", save_path, active_sandbox.name);
    FILE *pInfo = fopen(path, "w");

    if(pInfo != NULL){
        fprintf(pInfo, \
            "coordx:%d\ncoordy:%d\noffsetx:%d\noffsety:%d\n",
            player_coordinate.x,
            player_coordinate.y,
            player_coordinate_offset.x,
            player_coordinate_offset.y
        );
        fclose(pInfo);
    }
}

void ReadPlayerInfo(){
    char path[260];
    sprintf(path, "%s%s/player.dat", save_path, active_sandbox.name);
    FILE *pInfo = fopen(path, "r");

    if(pInfo != NULL){
        char *buffer;
        int length = GetLineLength(pInfo);

        buffer = malloc(sizeof(char) * length);
        while(fgets(buffer, length, pInfo)){

            char token[256];
            strcpy(token, strtok(buffer, ":"));
            if(strcmp(token, "coordx") == 0){
                player_coordinate.x = strtol(strtok(NULL, "\n"), NULL, 10);
            }
            if(strcmp(token, "coordy") == 0){
                player_coordinate.y = strtol(strtok(NULL, "\n"), NULL, 10);
            }
            if(strcmp(token, "offsetx") == 0){
                player_coordinate_offset.x = strtol(strtok(NULL, "\n"), NULL, 10);
            }
            if(strcmp(token, "offsety") == 0){
                player_coordinate_offset.y = strtol(strtok(NULL, "\n"), NULL, 10);
            }

            length = GetLineLength(pInfo);
            buffer = malloc(sizeof(char) * length);
        }
        fclose(pInfo);
        free(buffer);
    }else{
        // player_coordinate.x = 0;
        // player_coordinate.y = 0;
        // player_coordinate_offset.x = 0;
        // player_coordinate_offset.y = 0;
    }
}

void WriteSandboxInfo(){
    char path[260];
    sprintf(path, "%s%s/sandbox.dat", save_path, active_sandbox.name);
    FILE *sInfo = fopen(path, "w");

    if(sInfo != NULL){
        fprintf(sInfo, "seed:%lu\n", active_sandbox.seed);
        fclose(sInfo);
    }
}

void ReadSandboxInfo(){
    
}

void WriteSandbox(char *name){
    /**
     ** Writes 'active_sandbox' to 'name' save folder **
     * 
     *  Write player.dat
     *  Write mods.conf
     *  Write all loaded chunks
     */
    if(active_sandbox.isActive){
        //Create folder shell
        char path[260];
        sprintf(path, "%s%s/", save_path, name);
        _mkdir(path);
        sprintf(path, "%s%s/chunks/", save_path, name);
        _mkdir(path);

        WritePlayerInfo();
        WriteSandboxInfo();
        for(int i = 0; i < active_sandbox.chunk_buffer_size; i++){
            WriteChunk(&active_sandbox.chunk_buffer[i], active_sandbox.chunk_buffer[i].position);
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
     *  Read new sandbox into 'active_sandbox'
     *  Read player.dat
     *  Read mods.conf
     */
    // if()
    // UnloadSandbox();
    // NewSandbox(name, 334);
    // active_sandbox.id = 0;
    active_sandbox.name = malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(active_sandbox.name, name);
    active_sandbox.isActive = true;
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
    if(active_sandbox.isActive){
        DebugLog(D_ACT, "Unloading sandbox with name '%s'", active_sandbox.name);
        WriteSandbox(active_sandbox.name);
        active_sandbox.chunk_buffer_size = 0;
        free(active_sandbox.chunk_buffer);
        DebugLog(D_ACT, "Unloaded sandbox with name '%s', took %ums", active_sandbox.name, SDL_GetTicks() - start);
    }else{
        DebugLog(D_WARN, "Tried to unload sandbox when no sandbox loaded");
    }
    active_sandbox = undefined_sandbox;
}

void RenderSandbox(){
    iVector2 chunk = {0, 0};
    iVector2 chunk_offset = {0, 0};
    for(int y = 0; y <= chunk_load_radius * 2 - 1; y++){
        for(int x = 0; x <= chunk_load_radius * 2 - 1; x++){
            chunk = (iVector2){((player_coordinate.x) / chunk_size) + x - 1, ((player_coordinate.y) / chunk_size) + y - 1};
            chunk_offset.x = (x * chunk_size * tile_render_size) + (-player_coordinate.x % chunk_size) * tile_render_size - player_coordinate_offset.x;
            chunk_offset.y = (y * chunk_size * tile_render_size) + (-player_coordinate.y % chunk_size) * tile_render_size - player_coordinate_offset.y;
            
            chunk_offset.x += (SCREEN_WIDTH / 2) - (chunk_size * tile_render_size * chunk_load_radius) - tile_render_size / 2;
            chunk_offset.y += (SCREEN_HEIGHT / 2) - (chunk_size * tile_render_size * chunk_load_radius) - tile_render_size / 2;

            RenderChunk(FindChunk(chunk), chunk_offset);
        }
    }
    UnloadOldChunks();
    RenderCursor();
    CheckBufferGeneration();
}






BlockObject *PlaceBlock(BlockObject *block, iVector2 position, int layer, int rotation){//Returns whether or not the block was placed

    iVector2 chunk_offset = {position.x % chunk_size, position.y % chunk_size};
    iVector2 chunk_coord = {position.x / chunk_size, position.y / chunk_size};
    //Negative position
    if(position.x < 0){
        chunk_offset.x = (position.x + 1) % chunk_size + chunk_size - 1;
        chunk_coord.x = (position.x + 1) / chunk_size - 1;
    }
    if(position.y < 0){
        chunk_offset.y = (position.y + 1) % chunk_size + chunk_size - 1;
        chunk_coord.y = (position.y + 1) / chunk_size - 1;
    }
    BlockObject *tmp_block = FindChunk(chunk_coord)->tile[layer][chunk_offset.y][chunk_offset.x].block;
    FindChunk(chunk_coord)->tile[layer][chunk_offset.y][chunk_offset.x].block = block;
    FindChunk(chunk_coord)->tile[layer][chunk_offset.y][chunk_offset.x].rotation = rotation;
    return FindBlock_id(tmp_block->id);
}

void RenderCursor(){
    iVector2 chunkCenterOffset = {
        (SCREEN_WIDTH / 2) - (chunk_size * tile_render_size * chunk_load_radius) - tile_render_size / 2,
        (SCREEN_HEIGHT / 2) - (chunk_size * tile_render_size * chunk_load_radius) - tile_render_size / 2
    };

    mouse_tile_pos.x = (player_coordinate_offset.x + mouse_pos.x - chunkCenterOffset.x % tile_render_size) / tile_render_size;
    mouse_tile_pos.y = (player_coordinate_offset.y + mouse_pos.y - chunkCenterOffset.y % tile_render_size) / tile_render_size;
    SDL_Rect cursor = {
        mouse_tile_pos.x * tile_render_size + chunkCenterOffset.x % tile_render_size - player_coordinate_offset.x, 
        mouse_tile_pos.y * tile_render_size + chunkCenterOffset.y % tile_render_size - player_coordinate_offset.y, 
        tile_render_size, 
        tile_render_size
    };

    // PushRender_Tilesheet(renderer, FindTilesheet("builtin"), 2, cursor, RNDR_UI);
	RenderTilesheet(builtin_tilesheet, 2, &cursor, RNDR_UI, (Vector4){1, 1, 1, 1});

    mouse_global_tile_pos = (iVector2){
        mouse_tile_pos.x + player_coordinate.x - chunkCenterOffset.x / tile_render_size - chunk_size, 
        mouse_tile_pos.y + player_coordinate.y - chunkCenterOffset.y / tile_render_size - chunk_size
    };
    
    iVector2 info_group = {0, 0};
    RenderText(FindFont("default_font"), 1, info_group.x, info_group.y, 0, "mouse_tile_pos: %d, %d", mouse_tile_pos.x, mouse_tile_pos.y);
    RenderText(FindFont("default_font"), 1, info_group.x, info_group.y + 20, 0, "mouse_global_tile_pos: %d, %d", mouse_global_tile_pos.x, mouse_global_tile_pos.y);
    RenderText(FindFont("default_font"), 1, info_group.x, info_group.y + 40, 0, "player_coordinate_offset: %d, %d", player_coordinate_offset.x, player_coordinate_offset.y);
    RenderText(FindFont("default_font"), 1, info_group.x, info_group.y + 60, 0, "player_coordinate: %d, %d", player_coordinate.x, player_coordinate.y);
    iVector2 chunk_out = {0, 0};
    iVector2 chunk_offset = {0, 0};
    CoordinateConvert(mouse_global_tile_pos, &chunk_out, &chunk_offset);
    RenderText(FindFont("default_font"), 1, info_group.x, info_group.y + 80, 0, "chunk: %d, %d", chunk_out.x, chunk_out.y);
}

void LevelMouseInteraction(EventData event){
    if(mouse_held && active_sandbox.isActive){
        if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)){
            PlaceBlock(FindBlock("water"), mouse_global_tile_pos, 0, 0);
        }
        if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)){
            PlaceBlock(FindBlock("air"), mouse_global_tile_pos, 0, 0);
        }
	}
}

void LevelMovement(EventData event){
    // int x = 0;
    // int y = 0;
    int speed = 3;
    if(event.keyStates[SDL_SCANCODE_W]){
		global_position.y -= speed * deltatime;
        // y = -1;
	}
	if(event.keyStates[SDL_SCANCODE_S]){
		global_position.y += speed * deltatime;
        // y = 1;
	}
	if(event.keyStates[SDL_SCANCODE_A]){
		global_position.x -= speed * deltatime;
        // x = -1;
	}
	if(event.keyStates[SDL_SCANCODE_D]){
		global_position.x += speed * deltatime;
        // x = 1;
	}

    // if(abs((int)player_coordinate_offset.x) + 1 > tile_render_size / 2){
    //     player_coordinate.x += (player_coordinate_offset.x / (tile_render_size / 2));
    //     // player_coordinate_offset.x = (-((int)player_coordinate_offset.x % tile_render_size) + x) % (tile_render_size / 2);
    //     player_coordinate_offset.x = (-(((int)player_coordinate_offset.x >> 5) & 1) + x) % (tile_render_size / 2);
    // }
    // if(abs((int)player_coordinate_offset.y) + 1 > tile_render_size / 2){
    //     player_coordinate.y += (player_coordinate_offset.y / (tile_render_size / 2));
    //     player_coordinate_offset.y = (-((int)player_coordinate_offset.y % tile_render_size) + y) % (tile_render_size / 2);
    // }
    player_coordinate_offset.x = global_position.x % tile_render_size;
    player_coordinate_offset.y = global_position.y % tile_render_size;
    player_coordinate.y = global_position.y / tile_render_size;
    player_coordinate.x = global_position.x / tile_render_size;
}