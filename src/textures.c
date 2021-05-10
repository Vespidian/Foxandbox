#include "global.h"
#include "renderer/renderer.h"
#include "textures.h"

TextureObject *texture_stack;
unsigned int num_textures = 0;
static unsigned int nextID = 0;

TextureObject undefined_texture;

void InitTextures(){
    undefined_texture = (TextureObject){"undefined", -1, IMG_LoadTexture(renderer, "../images/undefined.png"), 16, 16};
    DebugLog(D_ACT, "Initialized texture subsystem");
}

TextureObject *LoadTexture(SDL_Renderer *renderer, const char *path, char *name){
    texture_stack = realloc(texture_stack, sizeof(TextureObject) * (num_textures + 1));
    texture_stack[num_textures].texture = IMG_LoadTexture(renderer, path);
    if(texture_stack[num_textures].texture == NULL){
        DebugLog(D_ERR, "Could not load texture id '%d' from path '%s'", nextID, path);
    }else{
        DebugLog(D_ACT, "Loaded texture id '%d' with name '%s' from '%s'", nextID, name, path);
    }

    texture_stack[num_textures].name = malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(texture_stack[num_textures].name, name);

    SDL_QueryTexture(texture_stack[num_textures].texture, NULL, NULL, &texture_stack[num_textures].w, &texture_stack[num_textures].h);
    texture_stack[num_textures].id = nextID;
    num_textures++;
    nextID++;
    return &texture_stack[num_textures - 1];
}

TextureObject *FindTexture(char *name){
    for(int i = 0; i < num_textures; i++){
        if(strcmp(texture_stack[i].name, name) == 0){
            return &texture_stack[i];
        }
    }
    return &undefined_texture;
}

TextureObject *IDFindTexture(unsigned int id){
    for(int i = 0; i < num_textures; i++){
        if(texture_stack[i].id == id){
            return &texture_stack[i];
        }
    }
    return &undefined_texture;
}