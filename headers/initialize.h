#ifndef INITIALIZE_H_
#define INITIALIZE_H_

extern SDL_Window* gWindow;
// The window renderer
extern SDL_Renderer* gRenderer;

extern SDL_Texture *tileSheetTex;
extern SDL_Texture *characterTex;

// extern bool init();
void Quit();

extern int WIDTH;
extern int HEIGHT;


#endif