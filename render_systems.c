#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


#include "headers/DataTypes.h"
#include "headers/ECS.h"
#include "headers/render_systems.h"
#include "headers/initialize.h"
#include "headers/lua_systems.h"
#include "headers/data.h"

int particleCount = 0;

int renderItemIndex = 0;
RenderComponent *renderBuffer;

TilesheetComponent undefinedSheet;
TilesheetComponent fontSheet;

int InitializeRenderer(SDL_Renderer *renderer){
	undefinedSheet = (TilesheetComponent){"undefined", IMG_LoadTexture(renderer, "images/undefined.png"), 16, 1, 1};
	fontSheet = (TilesheetComponent){"font", IMG_LoadTexture(renderer, "fonts/font.png"), 16, 12, 8};
}

void ResetRenderFrame(){//Clear and allocate render buffer + reset render counter
	free(renderBuffer);
	renderItemIndex = 0;
	renderBuffer = malloc(sizeof(RenderComponent));
}

int AddToRenderQueue(SDL_Renderer *renderer, TilesheetComponent *tileSheet, int tileNum, SDL_Rect destRect, int alpha, int zPos){
	return AddToRenderQueueEx(renderer, tileSheet, tileNum, destRect, alpha, zPos, 0, (SDL_Color){255, 255, 255});
}

int AddToRenderQueueEx(SDL_Renderer *renderer, TilesheetComponent *tileSheet, int tileNum, SDL_Rect destRect, int alpha, int zPos, int rotation, SDL_Color colorMod){
	if(alpha == -1){
		alpha = 255;
	}
	if(tileNum <= tileSheet->w * tileSheet->h - 1){
		renderBuffer = realloc(renderBuffer, (renderItemIndex + 1) * sizeof(RenderComponent));
		renderBuffer[renderItemIndex] = (RenderComponent){renderer, tileSheet, tileNum, destRect, alpha, zPos, rotation, colorMod};
		renderItemIndex++;
	}else if(strcmp(tileSheet->name, "undefined") == 0 || tileSheet->tex == NULL || tileNum < 0){
		renderBuffer = realloc(renderBuffer, (renderItemIndex + 1) * sizeof(RenderComponent));
		renderBuffer[renderItemIndex] = (RenderComponent){renderer, &undefinedSheet, 0, destRect, 255, zPos, 0, (SDL_Color){255, 255, 255}};
		renderItemIndex++;
	}else{
		printf("Error: Tile index not in image bounds!\n");
		return 1;
	}
	return 0;
}

void RenderUpdate(){
	int key, j; //Insertion sort
	RenderComponent tmpRenderItem;
	for (int i = 1; i < renderItemIndex; i++) {
		tmpRenderItem = renderBuffer[i];
		key = renderBuffer[i].zPos;
		j = i - 1;
		while (j >= 0 && renderBuffer[j].zPos > key) {
			renderBuffer[j + 1] = renderBuffer[j];
			j--;
		}
		renderBuffer[j + 1] = tmpRenderItem;
	}
	
	Vector2 tileInSheet;
	for(int i = 0; i < renderItemIndex; i++){
		tileInSheet.x = (renderBuffer[i].tile % renderBuffer[i].tileSheet->w) * renderBuffer[i].tileSheet->tile_size;
		tileInSheet.y = (renderBuffer[i].tile / renderBuffer[i].tileSheet->w) * renderBuffer[i].tileSheet->tile_size;
		
		SDL_SetTextureAlphaMod(renderBuffer[i].tileSheet->tex, renderBuffer[i].alpha);
		SDL_Rect sourceRect = {tileInSheet.x, tileInSheet.y, renderBuffer[i].tileSheet->tile_size, renderBuffer[i].tileSheet->tile_size};

		SDL_SetTextureColorMod(renderBuffer[i].tileSheet->tex, renderBuffer[i].colorMod.r, renderBuffer[i].colorMod.g, renderBuffer[i].colorMod.b);
		SDL_RenderCopyEx(renderBuffer[i].renderer, renderBuffer[i].tileSheet->tex, &sourceRect, &renderBuffer[i].transform, renderBuffer[i].rotation * 90, NULL, SDL_FLIP_NONE);
		SDL_SetTextureColorMod(renderBuffer[i].tileSheet->tex, 255, 255, 255);

	}
	ResetRenderFrame();
}

/**
 *  TEXT RENDERING
 */
 
int RenderText(SDL_Renderer *renderer, char *text, int x, int y, SDL_Color colorMod){
	int tracking = 9;//Spacing between letters
	int spacing = 16;
	
	if(text == NULL){
		printf("Error: RenderText called with null text field\n");
		return 1;
	}
	SDL_Rect screenRect = {0, 0, WIDTH, HEIGHT};
	
	SDL_Rect charRect = {x, y, spacing, spacing};
	if(strlen(text) > 0){//Make sure there is a string to display
		for(int i = 0; i < strlen(text); i++){//Iterate through the characters of the string
			int charVal = (int)text[i] - (int)' ';//Get an integer value from the character to be drawn
			//Non character cases
			if(charVal >= 0){//NOT SPACE
				if(SDL_HasIntersection(&charRect, &screenRect)){//Only render if text is on screen
					AddToRenderQueueEx(renderer, &fontSheet, charVal, charRect, -1, RNDRLYR_TEXT, 0, colorMod);
				}
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
	return 0;
}
int RenderText_d(SDL_Renderer *renderer, char *text, int x, int y){
	SDL_Color defaultColor = {255, 255, 255, 0xff};
	return RenderText(renderer, text, x, y, defaultColor);
}

/**
 *  PARTICLE SYSTEM
 */

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
	pSystem->pSheet = &particleSheet;//Use the particles sheet by default
	
	//Allocate the size of the particle array
	if(particleNum <= particleCap){
		pSystem->particles = malloc(sizeof(ParticleComponent) * particleNum + 1);
	}else{
		pSystem->particles = malloc(sizeof(ParticleComponent) * particleCap + 1);
	}

	for(int i = 0; i < pSystem->maxParticles; i++){
		particleCount++;
		if(particleCount <= particleCap){
			// pSystem->particles = realloc(pSystem->particles, (i + 1) * sizeof(ParticleComponent));//Allocate the space for the particle
			SpawnParticle(&pSystem->particles[i], area, xR, yR, duration);
		}else{
			break;
		}
	}
}

void DestroyParticleSystem(ParticleSystem *pSystem){
	free(pSystem->particles);
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
							AddToRenderQueue(renderer, system.pSheet, system.pType, particle.pos, (particle.duration * 255) / particle.initDuration, RNDRLYR_UI - 5);
						}else{
							AddToRenderQueue(renderer, system.pSheet, system.pType, particle.pos, 255, RNDRLYR_UI - 5);
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

bool DrawButton(SDL_Renderer *renderer, char *text, SDL_Rect rect){
	bool isClicked = false;
	
	SDL_Point mouse; 
	SDL_GetMouseState(&mouse.x, &mouse.y);
	if(SDL_PointInRect(&mouse, &rect)){
		rect.x -= 2;
		rect.y -= 2;
		rect.w += 4;
		rect.h += 4;
		if(mouseClicked){
			isClicked = true;
		}
	}
	AddToRenderQueue(renderer, find_tilesheet("ui"), 0, rect, 255, RNDRLYR_UI);
	Vector2 textPos = {(rect.x + rect.w / 2) - (strlen(text) * 10) / 2, (rect.y + rect.h / 2) - 8};
	RenderText_d(renderer, text, textPos.x, textPos.y);
	
	return isClicked;
}

void RenderPauseMenu(){
	// SDL_Rect tmp1 = {WIDTH / 2 - 64, HEIGHT / 2 - 16, 128, 32};
	// SDL_Rect tmp2 = {WIDTH / 2 - 32, 336, 128, 32};
	// if(DrawButton(renderer, "Button 1", tmp1)){
	// 	printf("wow 1\n");
	// }
	// if(DrawButton(renderer, "Button 2", tmp2)){
	// 	printf("wow 2\n");
	// }
}