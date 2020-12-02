#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "global.h"
#include "debug.h"
#include "render/renderer.h"
#include "event.h"

int loopStartTicks = 0;
float deltatime = 0;
int targetFramerate = 60;

bool running = true;

void Quit();

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

void GameLoop(){
	
}

void MenuLoop(){
	
}

int main(int argc, char *argv[]){
	SetupDebug();
	SetupSDL();
	
	printf("here we go.. waho!\n");
	while(running){
		loopStartTicks = SDL_GetTicks();
		FastEvents();
		while(SDL_PollEvent(&e) != 0){
			EventManager(&e);
		}
		
		SDL_Delay(1000 / targetFramerate);
		deltatime = SDL_GetTicks() - loopStartTicks / 10;
	}
	Quit();
	return 0;
}