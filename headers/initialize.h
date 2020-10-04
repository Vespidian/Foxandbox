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
enum DEBUG{D_ACT, D_WARN, D_ERR, D_SCRIPT_ERR, D_SCRIPT_ACT};
extern enum DEBUG debug;

/** 
 *  \brief Write formatted string to the log file 
 */
void DebugLog(int type, const char *format, ...);

void TextureInit();
void TextureDestroy();


extern SDL_Texture *undefinedTex;
extern TilesheetComponent undefinedSheet;

extern SDL_Texture *loadScreenTex;
extern SDL_Texture *backgroundTex;


extern SDL_Texture *fontTex;
extern TilesheetComponent fontSheet;


extern SDL_Texture *debugTex;
extern TilesheetComponent debugSheet;


extern SDL_Texture *particleTex;
extern TilesheetComponent particleSheet;


extern SDL_Texture *autotileMaskTex;
extern TilesheetComponent autotileMaskSheet;


extern SDL_Texture *InvertedAutotileMaskTex;
extern TilesheetComponent InvertedAutotileMaskSheet;


#endif