#ifndef DATA_H_
#define DATA_H_

extern SDL_Event e;

char *__strtok_r (char *s, const char *delim, char **save_ptr);

void SeedLehmer(int worldSeed, int x, int y);
extern uint32_t nLehmer;
uint32_t Lehmer32();

int getRnd(int min, int max);

bool GetClick(int x1, int y1, int x2, int y2);

void clearScreen(SDL_Renderer *renderer);

extern Vector2 mapOffsetPos;
extern Vector2 characterOffset;
extern Vector2 mousePos;
extern Vector2 midScreen;
int RenderText(SDL_Renderer *renderer, char *text, int x, int y, SDL_Color colorMod);
void RenderText_d(SDL_Renderer *renderer, char *text, int x, int y);
	
extern int tileSize;
extern int layerOrder;
extern bool enableHitboxes;

extern bool mouseUp;
extern bool uiMode;

char *strshft_l(char stringShift[128], int shiftBy);

extern int characterFacing;
void DrawCharacter(int direction, int numFrames);
// void PerlinInit();
// void PerlinNoise(int nCount, float *fSeed, int nOctaves, float *fOutput);
// extern int perlinNoise1D[];
// extern int noiseSeed1D[];
// extern const int noiseSize;


void RenderCursor();

#endif