#include "global.h"
#include "event.h"
#include "textures.h"
#include "render/tilesheet.h"
#include "entities/item.h"
#include "entities/block.h"
#include "render/renderer.h"
#include "render/render_text.h"
#include "level/level.h"

#include "level/level_generation.h"

int loopStartTicks = 0;
float deltatime = 0;
int targetFramerate = 60;

bool running = true;

void Quit();

void LoadBuiltinResources(){
	NewRawTilesheet("default_tilesheet", "../images/default_tilesheet.png", (Vector2){16, 16});
	NewRawTilesheet("builtin", "../images/builtin.png", (Vector2){16, 16});

	LoadTexture(renderer, "../images/testingTemp/tmpTilesheet.png", "tmp");
	NewTilesheet("tmp", FindTexture("tmp"), (Vector2){16, 16});
}

void Setup(){
	InitTextures();
	InitRenderer();
	InitTilesheets();
	InitItems();
	InitBlocks();
	InitFonts();
	InitLevels();

	LoadBuiltinResources();
    // printf("%s\n", fonts[0].tilesheet.name);
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
SDL_Rect tileDst = {0, 64	, 64, 64};
void GameLoop(){
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	// PushRenderEx(renderer, FindTexture("tmp"), src, dst, 0, 0, 255, (SDL_Color){255, 255, 255});
	// RenderText(renderer, FindFont("default_font"), 1, 100, 100, "testing 1234 . %d", 77314159);
	// RenderText(renderer, FindFont("default_font"), 2, 100, 100, "the quick brown fox jumped over the lazy dog\nTHE QUICK BROWN FOX JUMPED OVER THE LAZY DOG\n,./;'[]-=\\<>?:{}|_+!@#$%^&*()`~1234567890");
	// RenderText(renderer, FindFont("default_font"), 1, SCREEN_WIDTH - 150, 0, "Render calls: %d", renderQueueSize + 15);
	// PushRender_Tilesheet(renderer, FindTilesheet("default_font"), 1, tileDst, 0);
	RenderLevel();
	// printf("%d\n", sizeof(TileObject));
	RenderQueue();
	SDL_RenderPresent(renderer);
}

void MenuLoop(){
	
}

int main(int argc, char *argv[]){
	SetupDebug();
	SetupSDL();
	Setup();

	NewLevel("test", (Vector2){12, 12});
	SetActiveLevel(FindLevel("test"));

	NewBlock(NewItem("grass", FindTilesheet("tmp"), 1), NULL, FindTilesheet("tmp"), 1, false);
	NewBlock(NewItem("water", FindTilesheet("tmp"), 2), NULL, FindTilesheet("tmp"), 2, false);
	FillLevelLayer(&activeLevel, activeLevel.terrain, FindBlock("grass"));
	// RemoveBlock(activeLevel.terrain, (Vector2){0, 2});
	PlaceBlock(activeLevel.terrain, FindBlock("water"), (Vector2){0, 2}, 0);
	// RemoveBlock(activeLevel.terrain, (Vector2){0, 2});

	while(running){
		loopStartTicks = SDL_GetTicks();
		EventManager(&e);
		
		GameLoop();

		SDL_Delay(1000 / targetFramerate);
		deltatime = SDL_GetTicks() - loopStartTicks / 10;
	}
	Quit();
	return 0;
}