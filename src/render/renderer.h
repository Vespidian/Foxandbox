#ifndef RENDERER_H_
#define RENDERER_H_

#include "../textures.h"
#include "tilesheet.h"

enum renderLayers {RNDR_BACKGROUND = 0, RNDR_LEVEL = 5, RNDR_ENTITY = 20, RNDR_UI = 30, RNDR_TEXT = 40};

typedef struct RenderObject{
	SDL_Renderer *renderer;
	unsigned int texture;
	SDL_Rect src;
	SDL_Rect dst;
	int zPos;
	int rotation;
	int alpha;
	SDL_Color colorMod;
}RenderObject;

extern SDL_Window *window;
extern SDL_Renderer *renderer;

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

extern unsigned int renderQueueSize;

bool InitRenderer();

void PushRender_Tilesheet(SDL_Renderer *renderer, TilesheetObject *tilesheet, int index, SDL_Rect dst, int zPos);
void PushRender_TilesheetEx(SDL_Renderer *renderer, TilesheetObject *tilesheet, int index, SDL_Rect dst, int zPos, int rotation, uint8_t alpha, SDL_Color color);

void PushRenderEx(SDL_Renderer *renderer, TextureObject *texture, SDL_Rect src, SDL_Rect dst, int zPos, int rotation, uint8_t alpha, SDL_Color color);
void PushRender(SDL_Renderer *renderer, TextureObject *texture, SDL_Rect src, SDL_Rect dst, int zPos);

void RenderQueue();


#endif