#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../global.h"
#include "renderer.h"

SDL_Window *window;
SDL_Renderer *renderer;

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 800;

RenderObject *renderQueue;
int renderQueueSize = 0;

SDL_Texture *undefinedTexture = NULL;

bool InitRenderer(){
	bool success = true;
	
	undefinedTexture = IMG_LoadTexture(renderer, "../images/undefined.png");
	
	return success;
}

void DestroyRenderQueue(){
	
}

void ResetRenderQueue(){
	free(renderQueue);
	renderQueueSize = 0;
}

void CreateQueueSlot(){
	renderQueueSize++;
	renderQueue = realloc(renderQueue, sizeof(RenderObject) * (renderQueueSize + 1));
}

void PushRender_RawTilesheetEx(SDL_Renderer *renderer, SDL_Texture *tex, Vector2 tileSize, int index, SDL_Rect dst, int zPos, int rotation, uint8_t alpha, SDL_Color color){
	
}
void PushRender_RawTilesheet(SDL_Renderer *renderer, SDL_Texture *tex, Vector2 tileSize, int index, SDL_Rect dst, int zPos){//Creates a tilesheet of arbitrary size in texture
	
}

void PushRender_TilesheetEx(SDL_Renderer *renderer, TilesheetComponent *tilesheet, int index, SDL_Rect dst, int zPos, int rotation, uint8_t alpha, SDL_Color color){
	
}
void PushRender_Tilesheet(SDL_Renderer *renderer, TilesheetComponent *tilesheet, int index, SDL_Rect dst, int zPos){//Uses tilesheet component to render index of tilesheet
	
}


void PushRenderEx(SDL_Renderer *renderer, SDL_Texture *tex, SDL_Rect src, SDL_Rect dst, int zPos, int rotation, uint8_t alpha, SDL_Color color){
	CreateQueueSlot();
	renderQueue[renderQueueSize] = {
		renderer,
		tex,
		src,
		dst,
		zPos,
		rotation,
		alpha,
		color,
	};
}
void PushRender(SDL_Renderer *renderer, SDL_Texture *tex, SDL_Rect src, SDL_Rect dst, int zPos){//Pushes texture from specific coordinate
	PushRenderEx(SDL_Renderer *renderer, SDL_Texture *tex, SDL_Rect src, SDL_Rect dst, int zPos, 0, 255, (SDL_Color){255, 255, 255});
}


void SortRenderQueue(){
	
}

void RenderQueue(){
	SortRenderQueue();
	for(int i = 0; i < renderQueueSize; i++){
		if(renderQueue[i].tex == NULL){
			renderQueue[i].tex = undefinedTexture;
		}
		SDL_SetTextureAlphaMod(renderQueue[i].tex, renderQueue[i].alpha);
		SDL_SetTextureColorMod(renderQueue[i].tex, renderQueue[i].colorMod.r, renderQueue[i].colorMod.g, renderQueue[i].colorMod.b);
		SDL_RenderCopyEx(renderQueue[i].renderer, renderQueue[i].tex, &renderQueue[i].src, &renderQueue[i].dst, renderQueue[i].rotation, NULL, 0);
	}
	ResetRenderQueue();
}
