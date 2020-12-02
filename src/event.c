#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "global.h"

SDL_Event e;

void FastEvents(){
	const Uint8 *keyStates = SDL_GetKeyboardState(NULL);
	if(keyStates[SDL_SCANCODE_ESCAPE]){
		running = false;
	}
}

void EventManager(SDL_Event *e){
	switch(e->type){
		case SDL_QUIT:
			running = false;
	}
}