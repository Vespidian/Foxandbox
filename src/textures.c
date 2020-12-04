#include "global.h"
#include "render/renderer.h"
#include "textures.h"

TextureObject *textureStack;
unsigned int numTextures = 0;

TextureObject undefinedTexture;

void InitTextures(){
    undefinedTexture = (TextureObject){IMG_LoadTexture(renderer, "../images/undefined.png"), "undefined", 16, 16};
    DebugLog(D_ACT, "Initialized texture subsystem");
}

TextureObject *LoadTexture(SDL_Renderer *renderer, const char *path, char *name){
    textureStack = realloc(textureStack, sizeof(TextureObject) * (numTextures + 1));
    textureStack[numTextures].texture = IMG_LoadTexture(renderer, path);
    if(textureStack[numTextures].texture == NULL){
        DebugLog(D_ERR, "Could not load texture id '%d' from path '%s'", numTextures, path);
    }else{
        DebugLog(D_ACT, "Loaded texture id '%d' with name '%s' from '%s'", numTextures, name, path);
    }

    textureStack[numTextures].name = malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(textureStack[numTextures].name, name);

    SDL_QueryTexture(textureStack[numTextures].texture, NULL, NULL, &textureStack[numTextures].w, &textureStack[numTextures].h);
    numTextures++;
    return &textureStack[numTextures - 1];
}

TextureObject *FindTexture(char *name){
    for(int i = 0; i < numTextures; i++){
        if(strcmp(textureStack[i].name, name) == 0){
            return &textureStack[i];
        }
    }
    return NULL;
}