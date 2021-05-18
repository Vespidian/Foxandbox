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
#include "ui/load_screen.h"
#include "ui/elements/button.h"


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

TilesheetObject builtin_tilesheet;
TilesheetObject tmp_block_tilesheet;
void LoadBuiltinResources(){
	// NewTilesheetFromFile("default_tilesheet", "../images/default_tilesheet.png", (Vector2){16, 16});
	// NewTilesheetFromFile("builtin", "../images/builtin.png", (Vector2){16, 16});
	// LoadTexture("../images/loadScreen.png");

	// LoadTexture("../images/testingTemp/tmpTilesheet.png");
	tmp_block_tilesheet = LoadTilesheet("../images/testingTemp/tmpTilesheet.png", 16, 16);

	builtin_tilesheet = LoadTilesheet("../images/builtin.png", 16, 16);
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
	InitDebug();
	InitEvents();
	InitSDL();
	InitGL();

	InitRenderers();

	InitTextures();
	InitTilesheets();
	InitItems();
	InitBlocks();
	InitFonts();
	InitSandboxes();
	InitUI();
	LoadScreenInit();


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

	

	// RenderQuad(builtin_tilesheet.texture, NULL, NULL, 0, (Vector4){1, 1, 1, 1}, 0);
	// RenderTilesheet(builtin_tilesheet, 1, (SDL_Rect){100, 100, 100, 100}, 0, (Vector4){1, 1, 1, 1});
	// Button((SDL_Rect){100, 100, 100, 30}, "heyo this is a button");

	if(active_sandbox.isActive){
		RenderSandbox();
		// RenderGL();
	}else{
		RenderStartScreen();
	}
	// RenderText(renderer, FindFont("default_font"), 1, SCREEN_WIDTH - 200, 0, "Render calls: %d", renderQueueSize + 15);
	// PushRender_Tilesheet(renderer, FindTilesheet("builtin"), 5, (SDL_Rect){SCREEN_WIDTH - 15, 0, 15, deltatime * 2 + 10}, RNDR_UI);

	// RenderQueue();
	// SDL_RenderPresent(renderer);
	PushRender();
	SDL_GL_SwapWindow(window);
}

bool wireframe = false;
static void ToggleWireframe(){
	wireframe = !wireframe;
	if(wireframe){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}else{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

int main(int argc, char *argv[]){
	Setup();
	// startupTime.x = SDL_GetTicks();
	BindKeyEvent(ToggleWireframe, 'u', SDL_KEYDOWN);
	// NewBlock(NewItem("grass", FindTilesheet("tmp"), 1), NULL, FindTilesheet("tmp"), 1, false);
	NewBlock("grass", NULL, NULL, &tmp_block_tilesheet, 1, false, false);
	printf("name: %s\n", FindBlock("grass")->item.name);
	NewBlock("water", NULL, NULL, &tmp_block_tilesheet, 2, false, false);

	// printf("%s\n", block_stack[1].item.name);
	printf("block id: %d\n", FindBlock("grass")->id);
	printf("item id: %d\n", FindBlock("grass")->item.id);
	// NewBlock(NewItem("water", FindTilesheet("tmp"), 2), NULL, FindTilesheet("tmp"), 2, false);
	// NewBlock(NewItem("sand", FindTilesheet("tmp"), 3), NULL, FindTilesheet("tmp"), 3, false);

	// ReadSandbox("testing444");
	// RandomFill((Vector2){0, 0}, 50);
	// for(int i = 0; i < 7; i++){
		// IterateCellularAutomata((Vector2){-1, -1});
		// IterateCellularAutomata((Vector2){-1, -1});
		// IterateCellularAutomata((Vector2){-2, -1});
	// }
	SetupProcedural();
	active_sandbox.isActive = false;
	// FillArrayRandom1D();
	// GeneratePerlin1D();
	while(running){
		loop_start_ticks = SDL_GetTicks();
		EventListener();
		// LoadScreen();
		
		// if(active_sandbox.isActive){
			GameLoop();
		// }else{
			// MenuLoop();
			// RenderStartScreen();
		// }
		// ProceduralTesting();

		
		SDL_Delay(1000 / target_framerate);
		deltatime = (SDL_GetTicks() - loop_start_ticks) / 10;
	}
	Quit();
	return 0;
}