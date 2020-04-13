#ifndef DATA_H_
#define DATA_H_

extern SDL_Event e;

char *__strtok_r (char *s, const char *delim, char **save_ptr);

void SeedLehmer(int worldSeed, int x, int y);
extern uint32_t nLehmer;
uint32_t Lehmer32();

int getRnd(int min, int max);

bool GetClick(int x1, int y1, int x2, int y2);

extern Vector2 worldPosition;
extern Vector2 characterOffset;
extern Vector2 mousePos;
extern Vector2 midScreen;
void RenderText(char *text, int x, int y);

extern int tileSize;
extern int layerOrder;
extern bool enableHitboxes;

void randomArray();
extern int randArray[32][32];

extern bool mouseUp;


// void PerlinInit();
// void PerlinNoise(int nCount, float *fSeed, int nOctaves, float *fOutput);
// extern int perlinNoise1D[];
// extern int noiseSeed1D[];
// extern const int noiseSize;

#endif