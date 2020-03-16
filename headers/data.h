#ifndef DATA_H_
#define DATA_H_


char *__strtok_r (char *s, const char *delim, char **save_ptr);

void SeedLehmer(int worldSeed, int x, int y);
extern uint32_t nLehmer;
uint32_t Lehmer32();

int getRnd(int min, int max);

bool GetClick(int x1, int y1, int x2, int y2);

extern Vector2 worldPosition;
extern Vector2 mousePos;

#endif