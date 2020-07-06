#ifndef RENDERSYSTEMS_H_
#define RENDERSYSTEMS_H_

extern int particleCount;

void SpawnParticle(ParticleComponent *particle, SDL_Rect spawnArea, Range xR, Range yR, Range duration);
void NewParticleSystem(ParticleSystem *pSystem, int pType, SDL_Rect area, int particleNum, Range xR, Range yR, Range duration);
void RenderParticleSystem(ParticleSystem system);

int RenderText(SDL_Renderer *renderer, char *text, int x, int y, SDL_Color colorMod);
void RenderText_d(SDL_Renderer *renderer, char *text, int x, int y);
void RenderCursor();


#endif