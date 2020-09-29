#ifndef INITIALIZE_H_
#define INITIALIZE_H_

//Define the window and renderer
extern SDL_Window* window;
extern SDL_Renderer* renderer;

extern SDL_Window *consoleWindow;
extern SDL_Renderer *consoleRenderer;

bool init();
void Quit();

extern int WIDTH;
extern int HEIGHT;

extern FILE *logFile;
extern enum DEBUG{D_ACT, D_WARN, D_ERR, D_SCRIPT_ERR, D_SCRIPT_ACT};
// void DebugLog(int type, char *text);

/** 
 *  \brief Write formatted string to the log file 
 */
void DebugLog(int type, const char *format, ...);

void TextureInit();
void TextureDestroy();


extern SDL_Texture *undefinedTex;
extern WB_Tilesheet undefinedSheet;

extern SDL_Texture *loadScreenTex;
extern SDL_Texture *backgroundTex;


extern SDL_Texture *fontTex;
extern WB_Tilesheet fontSheet;


extern SDL_Texture *debugTex;
extern WB_Tilesheet debugSheet;


extern SDL_Texture *particleTex;
extern WB_Tilesheet particleSheet;


extern SDL_Texture *autotileMaskTex;
extern WB_Tilesheet autotileMaskSheet;


extern SDL_Texture *InvertedAutotileMaskTex;
extern WB_Tilesheet InvertedAutotileMaskSheet;


extern SDL_Texture *colorModTex;
extern WB_Tilesheet colorModSheet;


#endif