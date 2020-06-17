#ifndef INITIALIZE_H_
#define INITIALIZE_H_

//Define the window and renderer
extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;

bool init();
void Quit();

extern int WIDTH;
extern int HEIGHT;


void MapInit();
void TextureInit();
void TextureDestroy();


extern SDL_Texture *tileSheetTex;
extern WB_Tilesheet defSheet;


extern SDL_Texture *furnitureTex;
extern WB_Tilesheet furnitureSheet;


extern SDL_Texture *characterTex;
extern WB_Tilesheet characterSheet;


extern SDL_Texture *backgroundTex;
extern WB_Tilesheet backgroundSheet;


extern SDL_Texture *itemTex;
extern WB_Tilesheet itemSheet;


extern SDL_Texture *uiTex;
extern WB_Tilesheet uiSheet;


extern SDL_Texture *fontTex;
extern WB_Tilesheet fontSheet;


extern SDL_Texture *debugTex;
extern WB_Tilesheet debugSheet;


extern SDL_Texture *particleTex;
extern WB_Tilesheet particleSheet;



extern SDL_Texture *colorModTex;
extern WB_Tilesheet colorModSheet;


#endif