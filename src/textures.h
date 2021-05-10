#ifndef TEXTURES_H_
#define TEXTURES_H_

typedef struct TextureObject{
    char *name;
    // unsigned int gl_tex;
    unsigned int id;
	SDL_Texture *texture;
    int w;
    int h;
}TextureObject;


extern TextureObject *texture_stack;
extern TextureObject undefined_texture;


void InitTextures();
TextureObject *LoadTexture(SDL_Renderer *renderer, const char *path, char *name);
TextureObject *FindTexture(char *name);
TextureObject *IDFindTexture(unsigned int id);
#endif