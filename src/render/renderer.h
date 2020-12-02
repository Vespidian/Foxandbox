#ifndef RENDERER_H_
#define RENDERER_H_

typedef struct TilesheetComponent{
	char *name;
	SDL_Texture *tex;
	Vector2 tileSize;
	int w;
	int h;
}TilesheetComponent;


typedef struct RenderObject{
	SDL_Renderer *renderer;
	SDL_Texture *tex;
	SDL_Rect src;
	SDL_Rect dst;
	int zPos;
	int rotation;
	int alpha;
	SDL_Color colorMod;
}RenderObject;

extern SDL_Window *window;
extern SDL_Renderer *renderer;

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;




#endif