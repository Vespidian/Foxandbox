#ifndef EVENT_H_
#define EVENT_H_

enum EventPollType {EV_QUICK = 0, EV_ACCURATE = 1};

typedef struct EventData{
	SDL_Event *e;
	const Uint8 *keyStates;
	const Uint32 *mouseState;
}EventData;

typedef void (*EV_Function)(EventData event);
typedef struct InputEvent{
	int pollType;
	Uint32 eventType;
	EV_Function function;
	bool isKeyPress;
	char keyCode;
	Uint8 scanCode;
}InputEvent;

void NewEvent(int pollType, Uint32 eventType, EV_Function function);
void BindQuickKeyEvent(EV_Function function, Uint8 scanCode);
void BindKeyEvent(EV_Function function, char key, Uint32 keyPressType);

extern SDL_Event e;
extern Vector2 mousePos;
extern bool mouseHeld;
extern bool mouseClicked;

void InitEvents();
void FastEvents();
void EventManager(SDL_Event *e);

#endif