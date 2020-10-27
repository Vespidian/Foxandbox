#ifndef render_systems_H_
#define render_systems_H_

enum zBufferOrder {RNDRLYR_MAP = 0, RNDRLYR_PLAYER = 5, RNDRLYR_UI = 20, RNDRLYR_INV_ITEMS = 25, RNDRLYR_TEXT = 30};
extern enum zBufferOrder zOrder;

extern int particleCount;

void ResetRenderFrame();
int AddToRenderQueue(SDL_Renderer *renderer, TilesheetComponent *tileSheet, int tileNum, SDL_Rect destRect, int alpha, int zPos);
int AddToRenderQueueEx(SDL_Renderer *renderer, TilesheetComponent *tileSheet, int tileNum, SDL_Rect destRect, int alpha, int zPos, int rotation, SDL_Color colorMod);
void RenderUpdate();

void SpawnParticle(ParticleComponent *particle, SDL_Rect spawnArea, Range xR, Range yR, Range duration);
void NewParticleSystem(ParticleSystem *pSystem, int pType, SDL_Rect area, int particleNum, Range xR, Range yR, Range duration);
void RenderParticleSystem(ParticleSystem system);

int RenderText(SDL_Renderer *renderer, char *text, int x, int y, SDL_Color colorMod);
int RenderText_d(SDL_Renderer *renderer, char *text, int x, int y);

extern int renderItemIndex;
extern int tilePixelSize;
extern const int tileStretchSize;

bool DrawButton(SDL_Renderer *renderer, char *text, SDL_Rect rect);
void DrawCheckbox(SDL_Renderer *renderer, bool *value, char *label, SDL_Rect rect);

void RenderPauseMenu();


#endif