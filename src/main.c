#include "global.h"
#include "event.h"
#include "textures.h"
#include "render/tilesheet.h"
#include "entities/item.h"
#include "entities/block.h"
#include "render/renderer.h"

int loopStartTicks = 0;
float deltatime = 0;
int targetFramerate = 60;

bool running = true;

void Quit();

void Setup(){
	InitTextures();
	InitRenderer();
	InitTilesheets();
	InitItems();
	InitBlocks();

	LoadTexture(renderer, "../images/testingTemp/tmpTilesheet.png", "tmp");
	CreateTilesheet("tmp", FindTexture("tmp"), (Vector2){16, 16});
}

void SetupSDL(){
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		DebugLog(D_ERR, "Could not initialize SDL. SDL_Error: %s", SDL_GetError());
	}
	window = SDL_CreateWindow("FoXandbox", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if(window == NULL){
		DebugLog(D_ERR, "SDL window could not be created. SDL_Error: %s", SDL_GetError());
	}else{
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if(renderer == NULL){
			DebugLog(D_ERR, "SDL renderer could not be created: %s", SDL_GetError());
		}
	}
	
}

void Quit(){
	DebugLog(D_ACT, "Shutting down!");
	
	SDL_Quit();
	QuitDebug();
}

SDL_Rect src = {0, 0, 128, 128};
SDL_Rect dst = {0, 0, 512, 512};
SDL_Rect tileDst = {0, 0, 64, 64};
void GameLoop(){
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	PushRenderEx(renderer, FindTexture("tmp"), src, dst, 0, 1, 255, (SDL_Color){255, 255, 255});
	// tileDst.x = 0;
	// PushRender_TilesheetEx(renderer, FindTilesheet("tmp"), 1, tileDst, 0, 0, 255, (SDL_Color){255, 255, 255});
	// tileDst.x += 64;
	// PushRender_TilesheetEx(renderer, FindTilesheet("tmp"), 1, tileDst, 0, 90, 255, (SDL_Color){255, 255, 255});
	// tileDst.x += 64;
	// PushRender_TilesheetEx(renderer, FindTilesheet("tmp"), 1, tileDst, 0, 180, 255, (SDL_Color){255, 255, 255});
	// tileDst.x += 64;
	// PushRender_TilesheetEx(renderer, FindTilesheet("tmp"), 1, tileDst, 0, 45, 255, (SDL_Color){255, 255, 255});

	RenderQueue();
	SDL_RenderPresent(renderer);
}

void MenuLoop(){
	
}

int main(int argc, char *argv[]){
	SetupDebug();
	SetupSDL();
	Setup();
	
	while(running){
		loopStartTicks = SDL_GetTicks();
		FastEvents();
		while(SDL_PollEvent(&e) != 0){
			EventManager(&e);
		}
		
		GameLoop();

		SDL_Delay(1000 / targetFramerate);
		deltatime = SDL_GetTicks() - loopStartTicks / 10;
	}
	Quit();
	return 0;
}