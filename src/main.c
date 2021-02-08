#include "global.h"
#include "event.h"
#include "textures.h"
#include "entities/item.h"
#include "world/block.h"
#include "world/sandbox.h"
#include "render/tilesheet.h"
#include "render/renderer.h"
#include "render/render_text.h"
#include "ui/ui.h"
// #include "ui/ui_elements.h"
#include "ui/start_screen.h"


#include "animation/animation.h"
#include "ui/load_screen.h"

#include "world/sandbox_generation.h"

int loopStartTicks = 0;
float deltatime = 0;
int targetFramerate = 60;

bool running = true;

bool isDebug = true;

void Quit();

void LoadBuiltinResources(){
	NewRawTilesheet("default_tilesheet", "../images/default_tilesheet.png", (Vector2){16, 16});
	NewRawTilesheet("builtin", "../images/builtin.png", (Vector2){16, 16});
	LoadTexture(renderer, "../images/loadScreen.png", "loadscreen");

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
	InitEvents();
	InitSandboxes();
	InitUI();

	LoadBuiltinResources();
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
	// UnloadSandbox();
	running = false;
	
	SDL_Quit();
	QuitDebug();
}

// bool tmp = true;
// Vector2 startupTime = {0, 0};
SDL_Rect src = {0, 0, 128, 128};
SDL_Rect dst = {0, 128, 0, 64};
SDL_Rect tileDst = {0, 64, 64, 64};
void GameLoop(){
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	// dst.x += IntegerLerp(dst.x, 736, 10000);
	// dst.w += IntegerLerp(dst.w, 64, 10000);
	// PushRender_Tilesheet(renderer, FindTilesheet("builtin"), 2, dst, 1000);
	// RenderChunk(FindChunk((Vector2){0, 0}), (Vector2){0, 0});
	RenderSandbox();

	RenderText(renderer, FindFont("default_font"), 1, SCREEN_WIDTH - 200, 0, "Render calls: %d", renderQueueSize + 15);
	PushRender_Tilesheet(renderer, FindTilesheet("builtin"), 5, (SDL_Rect){SCREEN_WIDTH - 15, 0, 15, deltatime * 2 + 10}, RNDR_UI);

	RenderQueue();
	SDL_RenderPresent(renderer);
	// if(tmp){
	// 	startupTime.y = SDL_GetTicks();
	// 	printf("%d milliseconds to first render\n", startupTime.y - startupTime.x);
	// 	DebugLog(D_VERBOSE_ACT, "Time to end of first render: %ums", startupTime.y - startupTime.x);
	// 	tmp = false;
	// }
}

void MenuLoop(){
	
}

void LoadSUND(){
	ReadSandbox("terst");
}

int main(int argc, char *argv[]){
	SetupDebug();
	SetupSDL();
	// startupTime.x = SDL_GetTicks();
	Setup();

	NewBlock(NewItem("grass", FindTilesheet("tmp"), 1), NULL, FindTilesheet("tmp"), 1, false);
	NewBlock(NewItem("water", FindTilesheet("tmp"), 2), NULL, FindTilesheet("tmp"), 2, false);

	ReadSandbox("testing444");

	while(running){
		loopStartTicks = SDL_GetTicks();
		EventListener();
		LoadScreen();
		if(activeSandbox.isActive){
			GameLoop();
		}else{
			// MenuLoop();
			RenderStartScreen();
		}
		
		SDL_Delay(1000 / targetFramerate);
		deltatime = (SDL_GetTicks() - loopStartTicks) / 10;
	}
	Quit();
	return 0;
}