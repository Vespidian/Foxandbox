#include "../global.h"
#include "../utility.h"
#include "../textures.h"
#include "renderer.h"
#include "../debug.h"
#include "../utility.h"

SDL_Window *window;
SDL_Renderer *renderer;

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 800;

RenderObject *renderQueue;
unsigned int renderQueueSize = 0;


bool InitRenderer(){
	bool success = true;
		
	DebugLog(D_ACT, "Initialized rendering subsystem");
	return success;
}

void DestroyRenderQueue(){
	
}

void ResetRenderQueue(){
	if(renderQueueSize > 0){
		free(renderQueue);
	}
	renderQueue = malloc(sizeof(RenderObject));
	renderQueueSize = 0;
}

void NewQueueSlot(){
	renderQueueSize++;
	renderQueue = realloc(renderQueue, sizeof(RenderObject) * (renderQueueSize + 1));
}

void PushRender_RawTilesheetEx(SDL_Renderer *renderer, TextureObject *texture, Vector2 tileSize, int index, SDL_Rect dst, int zPos, int rotation, uint8_t alpha, SDL_Color color){
	if(SDL_HasIntersection(&dst, GetWindowRect(window))){

	}
}
void PushRender_RawTilesheet(SDL_Renderer *renderer, TextureObject *texture, Vector2 tileSize, int index, SDL_Rect dst, int zPos){//News a tilesheet of arbitrary size in texture
	
}

void PushRender_TilesheetEx(SDL_Renderer *renderer, TilesheetObject *tilesheet, int index, SDL_Rect dst, int zPos, int rotation, uint8_t alpha, SDL_Color color){
	if(SDL_HasIntersection(&dst, GetWindowRect(window))){
		NewQueueSlot();
		if(index > (tilesheet->tileSize).x * tilesheet->tileSize.y - 1){
			index = 0;
			tilesheet = &undefinedTilesheet;
		}
		SDL_Rect srcRect = {
			(index % (IDFindTexture(tilesheet->texture)->w / tilesheet->tileSize.x)) * tilesheet->tileSize.x,
			(index / (IDFindTexture(tilesheet->texture)->w / tilesheet->tileSize.x)) * tilesheet->tileSize.y,
			tilesheet->tileSize.x,
			tilesheet->tileSize.y,
		};
		renderQueue[renderQueueSize - 1] = (RenderObject){
			renderer,
			IDFindTexture(tilesheet->texture)->id,
			srcRect,
			dst,
			zPos,
			rotation,
			alpha,
			color,
		};
	}
}
void PushRender_Tilesheet(SDL_Renderer *renderer, TilesheetObject *tilesheet, int index, SDL_Rect dst, int zPos){//Uses tilesheet component to render index of tilesheet
	PushRender_TilesheetEx(renderer, tilesheet, index, dst, zPos, 0, 255, (SDL_Color){255, 255, 255});
}


void PushRenderEx(SDL_Renderer *renderer, TextureObject *texture, SDL_Rect src, SDL_Rect dst, int zPos, int rotation, uint8_t alpha, SDL_Color color){
	if(SDL_HasIntersection(&dst, GetWindowRect(window))){
		NewQueueSlot();
		renderQueue[renderQueueSize - 1] = (RenderObject){
			renderer,
			texture->id,
			src,
			dst,
			zPos,
			rotation,
			alpha,
			color,
		};
	}
}
void PushRender(SDL_Renderer *renderer, TextureObject *texture, SDL_Rect src, SDL_Rect dst, int zPos){//Pushes texture from specific coordinate
	PushRenderEx(renderer, texture, src, dst, zPos, 0, 255, (SDL_Color){255, 255, 255});
}

void SortRenderQueue(){
	int key, j;
	RenderObject tmpRenderObject;
	for(int i = 1; i < renderQueueSize; i++){
		tmpRenderObject = renderQueue[i];
		key = renderQueue[i].zPos;
		j = i - 1;

		while(j >= 0 && renderQueue[j].zPos > key){
			renderQueue[j + 1] = renderQueue[j];
			j--;
		}
		renderQueue[j + 1] = tmpRenderObject;
	}
}

void RenderQueue(){
	SortRenderQueue();
	for(int i = 0; i < renderQueueSize; i++){
		if(renderQueue[i].texture == undefinedTexture.id){
			IDFindTexture(renderQueue[i].texture)->texture = undefinedTexture.texture;
		}
		SDL_SetTextureAlphaMod(IDFindTexture(renderQueue[i].texture)->texture, renderQueue[i].alpha);
		SDL_SetTextureColorMod(IDFindTexture(renderQueue[i].texture)->texture, renderQueue[i].colorMod.r, renderQueue[i].colorMod.g, renderQueue[i].colorMod.b);
		SDL_RenderCopyEx(renderQueue[i].renderer, IDFindTexture(renderQueue[i].texture)->texture, &renderQueue[i].src, &renderQueue[i].dst, renderQueue[i].rotation, NULL, SDL_FLIP_NONE);
	}
	ResetRenderQueue();
}
