#ifndef TEXTURES_H_
#define TEXTURES_H_

typedef struct TextureObject{
    SDL_Texture *texture;
    char *name;
    int w;
    int h;
}TextureObject;


extern TextureObject *textureStack;
extern TextureObject undefinedTexture;


void InitTextures();
TextureObject *LoadTexture(SDL_Renderer *renderer, const char *path, char *name);
TextureObject *FindTexture(char *name);
#endif