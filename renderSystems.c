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
#include "headers/inventory.h"

const int particleCap = 10000;
int particleCount = 0;
RenderTileComponent **mouseEditingLayer;

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


int RenderText(SDL_Renderer *renderer, char *text, int x, int y, SDL_Color colorMod){
	int tracking = 9;//Spacing between letters
	int spacing = 16;
	
	if(text == NULL){
		printf("Error: RenderText called with null text field\n");
		return 1;
	}
	SDL_Rect screenRect = {0, 0, WIDTH, HEIGHT};
	SDL_SetTextureColorMod(fontSheet.tex, colorMod.r, colorMod.g, colorMod.b);
	
	SDL_Rect charRect = {x, y, spacing, spacing};
	if(strlen(text) > 0){//Make sure there is a string to display
		for(int i = 0; i < strlen(text); i++){//Iterate through the characters of the string
			int charVal = (int)text[i] - (int)' ';//Get an integer value from the character to be drawn
			//Non character cases
			if(charVal >= 0){//NOT SPACE
				if(SDL_HasIntersection(&charRect, &screenRect)){//Only render if text is on screen
					AddToRenderQueue(renderer, &fontSheet, charVal, charRect, -1, RNDRLYR_TEXT);
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
	SDL_SetTextureColorMod(fontSheet.tex, 255, 255, 255);
	return 0;
}
void RenderText_d(SDL_Renderer *renderer, char *text, int x, int y){
	SDL_Color defaultColor = {255, 255, 255, 0xff};
	RenderText(renderer, text, x, y, defaultColor);
}

void RenderCursor(){// Highlight the tile the mouse is currently on
	SDL_Rect mapRect = {-mapOffsetPos.x, -mapOffsetPos.y, 64 * activeLevel->map_size.x, 64 * activeLevel->map_size.y};
	// mouseTransform.tilePos = (Vector2){mouseTransform.screenPos.x, mouseTransform.screenPos.y};
	mouseTransform.tilePos.x = ((mouseTransform.screenPos.x + mapOffsetPos.x) / 64);
	mouseTransform.tilePos.y = ((mouseTransform.screenPos.y + mapOffsetPos.y) / 64);
	SDL_Point cursor = {(mouseTransform.tilePos.x * 64) - mapOffsetPos.x, (mouseTransform.tilePos.y * 64) - mapOffsetPos.y};
	if(SDL_PointInRect(&cursor, &mapRect) && !uiInteractMode){
		if((abs(character.transform.tilePos.x - mouseTransform.tilePos.x) <= reachDistance && abs(character.transform.tilePos.y - mouseTransform.tilePos.y) <= reachDistance) || !reachLimit){
		//Determine wether or not the user can reach infinitely
			AddToRenderQueue(renderer, find_tilesheet("ui"), 4, (SDL_Rect){cursor.x, cursor.y, 64, 64}, -1, RNDRLYR_UI - 10);
			
			//MouseText
			if(showDebugInfo){
				char screenPosT[256];
				snprintf(screenPosT, 1024, "mouseTransform.screenPos ->\nx: %d, y: %d", mouseTransform.tilePos.x, mouseTransform.tilePos.y);
				RenderText_d(renderer, screenPosT, 0, 96);
			}

			if(mouseHeld){//Place and remove tiles
				Vector2 tile = {mouseTransform.tilePos.x, mouseTransform.tilePos.y};
				//Only place the item if it is a block and the selected hotbar is occupied
				//Only place if the indicated block is different from the selected hotbar block
				if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)){
					/*if(invArray[selectedHotbar].occupied && invArray[selectedHotbar].item->isBlock){
						// INV_Subtract(1, invArray[selectedHotbar].item);
						if(strcmp(find_block(invArray[selectedHotbar].item->name)->layer, "terrain") == 0 && &invArray[selectedHotbar].item->name != &activeLevel->terrain[tile.y][tile.x].block->item->name){
							INV_Add(activeLevel->terrain[tile.y][tile.x].block->dropQty, activeLevel->terrain[tile.y][tile.x].block->dropItem);
							INV_WriteCell("sub", selectedHotbar, 1, invArray[selectedHotbar].item);
						}
						if(strcmp(find_block(invArray[selectedHotbar].item->name)->layer, "feature") == 0 && &invArray[selectedHotbar].item != &activeLevel->features[tile.y][tile.x].block->item){
							INV_Add(activeLevel->features[tile.y][tile.x].block->dropQty, activeLevel->features[tile.y][tile.x].block->dropItem);
							INV_WriteCell("sub", selectedHotbar, 1, invArray[selectedHotbar].item);
						}
						PlaceBlock(tile, find_block(invArray[selectedHotbar].item->name));
					}*/
					if(invArray[selectedHotbar].occupied && invArray[selectedHotbar].item->isBlock){
						if(&invArray[selectedHotbar].item->name != &mouseEditingLayer[tile.y][tile.x].block->item->name){
							INV_Add(mouseEditingLayer[tile.y][tile.x].block->dropQty, mouseEditingLayer[tile.y][tile.x].block->dropItem);
							INV_WriteCell("sub", selectedHotbar, 1, invArray[selectedHotbar].item);
							PlaceBlock(tile, find_block(invArray[selectedHotbar].item->name));
						}
					}
				}else if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)){
					if(mouseEditingLayer[tile.y][tile.x].block != find_block("air")){
						DebugLog(D_ACT, "Removed tile '%s' at %d,%d", mouseEditingLayer[tile.y][tile.x].block->item->name, tile.x, tile.y);
						INV_Add(activeLevel->terrain[tile.y][tile.x].block->dropQty, mouseEditingLayer[tile.y][tile.x].block->dropItem);
						mouseEditingLayer[tile.y][tile.x].block = find_block("air");
						if(activeLevel->features[tile.y][tile.x].block == find_block("air")){
							activeLevel->collision[tile.y][tile.x] = -1;
						}else{
							activeLevel->collision[tile.y][tile.x] = activeLevel->features[tile.y][tile.x].block->collisionType;
						}
					}
				}
			}
		}
	}
}

void RenderConsole(){
	int stringFit = 31;//Maximum number of characters to fit in the textbox
	int characterSpacing = 9;
	SDL_Rect console = {0, HEIGHT - 200, 300, 200};
	SDL_Rect textBox = {0, HEIGHT - 24, 300, 32};
	AddToRenderQueue(renderer, find_tilesheet("ui"), 0, console, 170, RNDRLYR_UI);//Console
	AddToRenderQueue(renderer, find_tilesheet("ui"), 0, textBox, 190, RNDRLYR_UI);//Text input

	if(strlen(currentCollectedText) < stringFit){
		if(inputMode == 1){
			RenderText_d(renderer, "_", strlen(currentCollectedText) * characterSpacing, HEIGHT - 20);//Cursor
		}
		RenderText_d(renderer, currentCollectedText, 0, HEIGHT - 20);
	}else{//Scroll text
		RenderText_d(renderer, "_", stringFit * characterSpacing, HEIGHT - 20);//Cursor
		RenderText_d(renderer, currentCollectedText, 278 - strlen(currentCollectedText) * characterSpacing, HEIGHT - 20);
	}

	RenderText_d(renderer, consoleOutput, console.x, console.y + 4);
}

bool DrawButton(SDL_Renderer *renderer, char *text, SDL_Rect rect){
	bool isClicked = false;
	
	if(SDL_PointInRect(&mouseTransform.screenPos, &rect)){
		if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)){
			isClicked = true;
		}
	}
	AddToRenderQueue(renderer, find_tilesheet("ui"), 0, rect, 255, RNDRLYR_UI);
	Vector2 textPos = {(rect.x + rect.w / 2) - (strlen(text) * 10) / 2, (rect.y + rect.h / 2) - 8};
	RenderText_d(renderer, text, textPos.x, textPos.y);
	
	return isClicked;
}


void RenderPauseMenu(){
	// SDL_Rect tmp1 = {WIDTH / 2 - 32, 300, 128, 32};
	// SDL_Rect tmp2 = {WIDTH / 2 - 32, 336, 128, 32};
	// if(DrawButton(renderer, "Button 1", tmp1)){
	// 	printf("wow 1\n");
	// }
	// if(DrawButton(renderer, "Button 2", tmp2)){
	// 	printf("wow 2\n");
	// }
}