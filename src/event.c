#include "global.h"
#include "render/renderer.h"
#include "level/level.h"

SDL_Event e;
Vector2 mousePos = {0, 0};

bool mouseHeld = false;
bool mouseClicked = false;

void MouseEvents(){
    if(SDL_GetMouseState(&mousePos.x, &mousePos.y)){
		mouseHeld = true;
	}

	LevelMouseInteraction();
}

void FastEvents(){
	const Uint8 *keyStates = SDL_GetKeyboardState(NULL);
	if(keyStates[SDL_SCANCODE_ESCAPE]){
		running = false;
	}

	if(keyStates[SDL_SCANCODE_W]){
		globalCoordinates.y -= 1;
	}
	if(keyStates[SDL_SCANCODE_S]){
		globalCoordinates.y += 1;
	}
	if(keyStates[SDL_SCANCODE_A]){
		globalCoordinates.x -= 1;
	}
	if(keyStates[SDL_SCANCODE_D]){
		globalCoordinates.x += 1;
	}

	MouseEvents();
}

void EventManager(SDL_Event *e){
	mouseClicked = false;
	mouseHeld = false;

	FastEvents();

	while(SDL_PollEvent(e) != 0){
		switch(e->type){
			case SDL_MOUSEBUTTONDOWN:
				if(e->key.state == SDL_RELEASED){
					mouseClicked = true;
				}
				break;
			case SDL_WINDOWEVENT:
				if(e->window.event == SDL_WINDOWEVENT_RESIZED){
					SDL_GetWindowSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
				}
				break;
			case SDL_QUIT:
				running = false;
		}
	}
}