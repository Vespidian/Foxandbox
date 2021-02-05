#include "global.h"
#include "render/renderer.h"
#include "event.h"

SDL_Event event;
Vector2 mousePos = {0, 0};

InputEvent *events;
int numEvents = 0;

bool enableInput = true;

bool mouseHeld = false;
bool mouseClicked = false;

//Event predefinitions
void MouseClicked();
void KeyEvents_quick();
void WindowResize();
// void SDLQuitEvent();

//Event management
void InitEvents(){
	events = malloc(sizeof(InputEvent));

	NewEvent(EV_ACCURATE, SDL_MOUSEBUTTONDOWN, MouseClicked);
	BindKeyEvent(Quit, 0x1B, SDL_KEYDOWN);//escape
	NewEvent(EV_ACCURATE, SDL_WINDOWEVENT, WindowResize);
	NewEvent(EV_ACCURATE, SDL_QUIT, Quit);

	DebugLog(D_ACT, "Initialized event subsystem");
}

void NewEvent(int pollType, Uint32 eventType, EV_Function function){
	events = realloc(events, sizeof(InputEvent) * (numEvents + 1));
	events[numEvents] = (InputEvent){pollType, eventType, function, false, 0x00};
	numEvents++;
}

void PollEvents(){
	const Uint8 *keyStates = SDL_GetKeyboardState(NULL);
	const Uint32 mouseState = SDL_GetMouseState(&mousePos.x, &mousePos.y);
	if(mouseState){
		mouseHeld = true;
	}
	while(SDL_PollEvent(&event)){
	// SDL_PollEvent(&event);
		for(int i = 0; i < numEvents; i++){
			if(events[i].pollType == EV_ACCURATE){
				if(events[i].eventType == event.type){
					if(events[i].isKeyPress){
						if(event.key.keysym.sym == events[i].keyCode){
							events[i].function((EventData){&event, keyStates, &mouseState});
						}
					}else{
						events[i].function((EventData){&event, keyStates, &mouseState});
					}
				}
			}
		}
	}
	for(int i = 0; i < numEvents; i++){
		if(events[i].pollType == EV_QUICK){
			if(events[i].isKeyPress){
				if(keyStates[events[i].scanCode]){
					events[i].function((EventData){NULL, keyStates, &mouseState});
				}
			}else{
				events[i].function((EventData){NULL, keyStates, &mouseState});
			}
		}
	}
}

void EventListener(){
	mouseClicked = false;
	mouseHeld = false;
	if(enableInput){
		PollEvents();
	}
}

void BindQuickKeyEvent(EV_Function function, Uint8 scanCode){
	NewEvent(EV_QUICK, SDL_KEYDOWN, function);
	events[numEvents - 1] = (InputEvent){EV_QUICK, SDL_KEYDOWN, function, true, 0x00, scanCode};
}

void BindKeyEvent(EV_Function function, char keyCode, Uint32 keyPressType){
	//Make sure keypresstype is keydown or keyup
	if(keyPressType != SDL_KEYDOWN || keyPressType != SDL_KEYUP){
		keyPressType = SDL_KEYDOWN;
	}
	NewEvent(EV_ACCURATE, keyPressType, function);
	events[numEvents - 1] = (InputEvent){EV_ACCURATE, keyPressType, function, true, keyCode};
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
		DebugLog(D_ACT, "Window resolution set to %dx%d", SCREEN_WIDTH, SCREEN_HEIGHT);
	}
}

void KeyEvents_quick(EventData event){
	if(event.keyStates[SDL_SCANCODE_ESCAPE]){
		running = false;
	}
}