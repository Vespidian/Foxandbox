#ifndef INITIALIZE_H_
#define INITIALIZE_H_

extern SDL_Window* gWindow;
// The window renderer
extern SDL_Renderer* gRenderer;

extern SDL_Texture *tileSheetTex;
extern SDL_Texture *furnitureTex;
extern SDL_Texture *characterTex;
extern SDL_Texture *backgroundTex;
extern SDL_Texture *itemTex;
extern SDL_Texture *uiTex;
extern SDL_Texture *fontTex;

extern WB_Tilesheet defSheet;
extern WB_Tilesheet furnitureSheet;
extern WB_Tilesheet characterSheet;
extern WB_Tilesheet backgroundSheet;
extern WB_Tilesheet itemSheet;
extern WB_Tilesheet uiSheet;
extern WB_Tilesheet fontSheet;

extern SDL_Texture *colorModTex;
// extern bool init();
void Quit();

extern int WIDTH;
extern int HEIGHT;

// extern TTF_Font *font;

void MapInit();
void TextureInit();
void TextureDestroy();

#endif