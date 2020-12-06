#include "global.h"
#include "render/renderer.h"

SDL_Event e;

void FastEvents(){
	const Uint8 *keyStates = SDL_GetKeyboardState(NULL);
	if(keyStates[SDL_SCANCODE_ESCAPE]){
		running = false;
	}
}

void EventManager(SDL_Event *e){
	switch(e->type){
		case SDL_WINDOWEVENT:
			if(e->window.event == SDL_WINDOWEVENT_RESIZED){
				SDL_GetWindowSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
			}
			break;
		case SDL_QUIT:
			running = false;
	}
}