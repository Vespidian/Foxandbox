#include "global.h"
#include "render/renderer.h"
#include "event.h"

SDL_Event e;
Vector2 mousePos = {0, 0};

bool mouseHeld = false;
bool mouseClicked = false;

InputEvent *events;
int numEvents = 0;

void MouseClicked();
void KeyEvents_quick();
void WindowResize();
void SDLQuitEvent();

void InitEvents(){
	events = malloc(sizeof(InputEvent));

	NewEvent(EV_ACCURATE, SDL_MOUSEBUTTONDOWN, MouseClicked);
	NewEvent(EV_QUICK, SDL_KEYDOWN, KeyEvents_quick);
	NewEvent(EV_ACCURATE, SDL_WINDOWEVENT, WindowResize);
	NewEvent(EV_ACCURATE, SDL_QUIT, SDLQuitEvent);

	DebugLog(D_ACT, "Initialized event subsystem");
}

void NewEvent(int pollType, Uint32 eventType, EV_Function function){
	events = realloc(events, sizeof(InputEvent) * (numEvents + 1));
	events[numEvents] = (InputEvent){pollType, eventType, function};
	numEvents++;
}

void PollEvents(){
	const Uint8 *keyStates = SDL_GetKeyboardState(NULL);
	const Uint32 mouseState = SDL_GetMouseState(&mousePos.x, &mousePos.y);
	if(mouseState){
		mouseHeld = true;
	}
	for(int i = 0; i < numEvents; i++){
		if(events[i].pollType == EV_QUICK){
			events[i].function((EventData){NULL, keyStates, &mouseState});
		}
	}
	while(SDL_PollEvent(&e) != 0){
		for(int i = 0; i < numEvents; i++){
			if(events[i].pollType == EV_ACCURATE){
				if(events[i].eventType == e.type){
					events[i].function((EventData){&e, NULL, NULL});
				}
			}
		}
	}
}

void EventManager(SDL_Event *e){
	mouseClicked = false;
	mouseHeld = false;
	PollEvents();
}


//Events

void MouseClicked(EventData event){
	if(event.e->key.state == SDL_RELEASED){
		mouseClicked = true;
	}
}

void WindowResize(EventData event){
	if(event.e->window.event == SDL_WINDOWEVENT_RESIZED){
		SDL_GetWindowSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
	}
}

void SDLQuitEvent(){
	running = false;
}

void KeyEvents_quick(EventData event){
	if(event.keyStates[SDL_SCANCODE_ESCAPE]){
		running = false;
	}
}