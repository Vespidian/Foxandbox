#ifndef EVENT_H_
#define EVENT_H_

extern SDL_Event e;
extern Vector2 mousePos;
extern bool mouseHeld;
extern bool mouseClicked;

void FastEvents();
void EventManager(SDL_Event *e);

#endif