#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "headers/DataTypes.h"
#include "headers/ECS.h"
#include "headers/initialize.h"
#include "headers/data.h"
#include "headers/tileMap.h"
#include "headers/mapGeneration.h"
#include "headers/renderSystems.h"

const int particleCap = 10000;
int particleCount = 0;

void SpawnParticle(ParticleComponent *particle, SDL_Rect spawnArea, Range xR, Range yR, Range duration){
	particle->active = true;//Activate the particle
	particle->initDuration = getRnd(duration.min, duration.max);
	particle->duration = particle->initDuration;
	particle->size = 4;
	particle->pos.x = getRnd(spawnArea.x, spawnArea.x + spawnArea.w);
	particle->pos.y = getRnd(spawnArea.y, spawnArea.y + spawnArea.h);
	particle->pos.w = particle->size;
	particle->pos.h = particle->size;
	
	particle->dir.x = getRnd(xR.min, xR.max + 1);
	particle->dir.y = getRnd(yR.min, yR.max + 1);
}

void NewParticleSystem(ParticleSystem *pSystem, int pType, SDL_Rect area, int particleNum, Range xR, Range yR, Range duration){
	// pSystem.area = (SDL_Rect){200, 200, 16, 16};
	// pSystem.area = (SDL_Rect){0, 0, WIDTH, HEIGHT};
	// pSystem.maxParticles = 25;
	// pSystem->sysDir = sysDir;
	pSystem->playSystem = true;//Play the system by default
	pSystem->pType = pType;
	pSystem->area = area;
	pSystem->fade = true;//Enable fade by default
	pSystem->maxParticles = particleNum;
	pSystem->xR = xR;
	pSystem->yR = yR;
	pSystem->duration = duration;
	pSystem->boundaryCheck = false;//Dont delete when outside boundary by default
	pSystem->pSheet = particleSheet;//Use the particles sheet by default
	
	pSystem->particles = malloc(sizeof(ParticleComponent));//Allocate the size of the particle array
	
	for(int i = 0; i < pSystem->maxParticles; i++){
		particleCount++;
		if(particleCount <= particleCap){
			pSystem->particles = realloc(pSystem->particles, (i + 1) * sizeof(ParticleComponent));//Allocate the space for the particle
			SpawnParticle(&pSystem->particles[i], area, xR, yR, duration);
		}else{
			break;
		}
	}
}

void RenderParticleSystem(ParticleSystem system){
	if(system.playSystem){
		for(int i = 0; i < system.maxParticles; i++){
			particleCount++;
			if(particleCount <= particleCap){
				ParticleComponent particle = (ParticleComponent)system.particles[i];
				if(particle.active){
					particle.pos.x += particle.dir.x * deltaTime;
					particle.pos.y += particle.dir.y * deltaTime;
					particle.duration -= 1;
					if(particle.duration <= 0){//Reset the particle
						particle.duration = 0;
						particle.active = false;
					}
					//If the boundarycheck is on and a particle is outside the bound, delete it
					if(system.boundaryCheck && !SDL_HasIntersection(&system.area, &particle.pos)){
						particle.duration = 0;
						particle.active = false;
					}
					if(particle.active){
						if(system.fade){
							AddToRenderQueue(gRenderer, system.pSheet, system.pType, particle.pos, (particle.duration * 255) / particle.initDuration, RNDRLYR_UI - 5);
						}else{
							AddToRenderQueue(gRenderer, system.pSheet, system.pType, particle.pos, 255, RNDRLYR_UI - 5);
						}
					}
					system.particles[i] = (ParticleComponent)particle;
				}else if(getRnd(0, 2) == 1){
					SpawnParticle(&system.particles[i], system.area, system.xR, system.yR, system.duration);
				}
			}else{
				break;
			}
		}
	}
}


int RenderText(SDL_Renderer *renderer, char *text, int x, int y, SDL_Color colorMod){
	int tracking = 9;//Spacing between letters
	int spacing = 16;
	
	if(text == NULL){
		printf("Error: RenderText called with null text field\n");
		return 1;
	}

	SDL_SetTextureColorMod(fontSheet.tex, colorMod.r, colorMod.g, colorMod.b);
	
	SDL_Rect charRect = {x, y, spacing, spacing};
	if(strlen(text) > 0){//Make sure there is a string to display
		for(int i = 0; i < strlen(text); i++){//Iterate through the characters of the string
			int charVal = (int)text[i] - (int)' ';//Get an integer value from the character to be drawn
			//Non character cases
			if(charVal >= 0){//SPACE
				AddToRenderQueue(renderer, fontSheet, charVal, charRect, -1, RNDRLYR_TEXT);
				charRect.x += tracking;
			}else if(charVal == -22){//NEWLINE (\n)
				charRect.y += spacing;
				charRect.x = x;
			}else if(charVal == -23){//TAB
				charRect.x += tracking * 4;
			}
		}
	}else if(strlen(text) < 1){
		// printf("Error: No text provided on RenderText()\n");
		return 1;
	}
	SDL_SetTextureColorMod(fontSheet.tex, 255, 255, 255);
	return 0;
}
void RenderText_d(SDL_Renderer *renderer, char *text, int x, int y){
	SDL_Color defaultColor = {255, 255, 255, 0xff};
	RenderText(renderer, text, x, y, defaultColor);
}

void RenderCursor(){// Highlight the tile the mouse is currently on
	SDL_Rect mapRect = {-mapOffsetPos.x, -mapOffsetPos.y, 64 * 32, 64 * 32};
	Vector2 mouseTile = {mousePos.x, mousePos.y};
	mouseTile.x = ((mousePos.x + mapOffsetPos.x) / 64);
	mouseTile.y = ((mousePos.y + mapOffsetPos.y) / 64);
	SDL_Point cursor = {(mouseTile.x * 64) - mapOffsetPos.x, (mouseTile.y * 64) - mapOffsetPos.y};
	if(SDL_PointInRect(&cursor, &mapRect) && !uiInteractMode){
		AddToRenderQueue(gRenderer, debugSheet, 0, (SDL_Rect){cursor.x, cursor.y, 64, 64}, -1, RNDRLYR_UI - 1);
		
		//MouseText
		char mousePosT[256];
		snprintf(mousePosT, 1024, "MOUSEPOS ->\nx: %d, y: %d", mouseTile.x, mouseTile.y);
		RenderText_d(gRenderer, mousePosT, 0, 96);
		
		if(mouseHeld){//Place and remove tiles
			if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)){
				TileMapEdit(buildLayer_tmp, (Vector2){mouseTile.x, mouseTile.y}, 0, false);
				TileMapEdit(buildLayer, (Vector2){mouseTile.x, mouseTile.y}, 0, false);
				AutotileMap(buildLayer);
			}else if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)){
				TileMapEdit(buildLayer, (Vector2){mouseTile.x, mouseTile.y}, 47, false);
				TileMapEdit(buildLayer_tmp, (Vector2){mouseTile.x, mouseTile.y}, 47, false);
				AutotileMap(buildLayer);
			}
		}
	}
}



bool DrawButton(SDL_Renderer *renderer, char *text, SDL_Rect rect){
	bool isClicked = false;
	
	return isClicked;
}