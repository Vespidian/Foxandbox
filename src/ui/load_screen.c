#include "../global.h"
#include "load_screen.h"
#include "../renderer/renderer.h"
#include "../animation/animation.h"
#include "../event.h"

#define LOADBARSIZE SCREEN_WIDTH / 3
bool loading = true;

int loadAmount = 0;
int duration = 40000;


void LoadScreen(){
	if(loadAmount == LOADBARSIZE){
		loading = false;
		enable_input = true;
	}
	if(loading){
		enable_input = false;
		PushRender(renderer, FindTexture("loadscreen"), (SDL_Rect){0, 0, 64, 64}, (SDL_Rect){SCREEN_WIDTH / 2 - 128, SCREEN_HEIGHT / 2 - 128, 256, 256}, RNDR_TEXT + 2);
		PushRender_TilesheetEx(renderer, FindTilesheet("builtin"), 0, (SDL_Rect){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, RNDR_TEXT + 1, 0, 255, (SDL_Color){31, 161, 143});
		loadAmount += IntegerLerp(loadAmount, LOADBARSIZE, 65536 - duration);
		PushRender_TilesheetEx(renderer, FindTilesheet("builtin"), 0, (SDL_Rect){SCREEN_WIDTH / 2 - LOADBARSIZE / 2, SCREEN_HEIGHT / 2 + 150, loadAmount, 20}, RNDR_TEXT + 1, 0, 255, (SDL_Color){158, 29, 46});
	}
}