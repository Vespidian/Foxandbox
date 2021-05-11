#include "global.h"
#include "event.h"
#include "gl_context.h"
#include "textures.h"
#include "entities/item.h"

#include "world/block.h"
#include "world/sandbox.h"

#include "renderer/tilesheet.h"
#include "renderer/renderer.h"
#include "renderer/render_text.h"
#include "renderer/render_text.h"
#include "renderer/quad.h"

#include "ui/ui.h"
#include "ui/start_screen.h"


#include "animation/animation.h"
#include "ui/load_screen.h"

#include "world/sandbox_generation.h"



#include "procedural_testing.h"

int loop_start_ticks = 0;
float deltatime = 0;
int target_framerate = 60;

int SCREEN_WIDTH =  800;
int SCREEN_HEIGHT =  800;
SDL_Window *window = NULL;

bool running = true;

void LoadBuiltinResources(){
	// NewRawTilesheet("default_tilesheet", "../images/default_tilesheet.png", (Vector2){16, 16});
	// NewRawTilesheet("builtin", "../images/builtin.png", (Vector2){16, 16});
	// LoadTexture("../images/loadScreen.png");

	// LoadTexture("../images/testingTemp/tmpTilesheet.png");
	// NewTilesheet("tmp", FindTexture("tmp"), (Vector2){16, 16});
}

void InitSDL(){
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		DebugLog(D_ERR, "Could not initialize SDL. SDL_Error: %s", SDL_GetError());
	}
	window = SDL_CreateWindow("FoXandbox", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if(window == NULL){
		DebugLog(D_ERR, "SDL window could not be created. SDL_Error: %s", SDL_GetError());
	}
}

void InitRenderers(){
	InitQuadRender();
}

void Setup(){
	InitSDL();
	InitGL();

	InitRenderers();

	// InitTextures();
	// InitTilesheets();
	// InitItems();
	// InitBlocks();
	// InitFonts();
	// InitEvents();
	// InitSandboxes();
	// InitUI();


	LoadBuiltinResources();
}

void Quit(){
	DebugLog(D_ACT, "Shutting down!");
	// UnloadSandbox();
	running = false;
	
	SDL_Quit();
	QuitDebug();
}

void GameLoop(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	// SDL_RenderClear(renderer);

	
	// RenderSandbox();
	RenderGL();

	// RenderText(renderer, FindFont("default_font"), 1, SCREEN_WIDTH - 200, 0, "Render calls: %d", renderQueueSize + 15);
	// PushRender_Tilesheet(renderer, FindTilesheet("builtin"), 5, (SDL_Rect){SCREEN_WIDTH - 15, 0, 15, deltatime * 2 + 10}, RNDR_UI);

	// RenderQueue();
	// SDL_RenderPresent(renderer);
	SDL_GL_SwapWindow(window);
}

int main(int argc, char *argv[]){
	SetupDebug();
	Setup();
	// startupTime.x = SDL_GetTicks();

	// NewBlock(NewItem("grass", FindTilesheet("tmp"), 1), NULL, FindTilesheet("tmp"), 1, false);
	// NewBlock(NewItem("water", FindTilesheet("tmp"), 2), NULL, FindTilesheet("tmp"), 2, false);
	// NewBlock(NewItem("sand", FindTilesheet("tmp"), 3), NULL, FindTilesheet("tmp"), 3, false);

	// ReadSandbox("testing444");
	// RandomFill((Vector2){0, 0}, 50);
	// for(int i = 0; i < 7; i++){
		// IterateCellularAutomata((Vector2){-1, -1});
		// IterateCellularAutomata((Vector2){-1, -1});
		// IterateCellularAutomata((Vector2){-2, -1});
	// }
	// SetupProcedural();
	// active_sandbox.isActive = false;
	// FillArrayRandom1D();
	// GeneratePerlin1D();
	while(running){
		loop_start_ticks = SDL_GetTicks();
		EventListener();
		LoadScreen();
		
		if(active_sandbox.isActive){
			GameLoop();
		}else{
			// MenuLoop();
			// RenderStartScreen();
		}
		// ProceduralTesting();

		
		SDL_Delay(1000 / target_framerate);
		deltatime = (SDL_GetTicks() - loop_start_ticks) / 10;
	}
	Quit();
	return 0;
}