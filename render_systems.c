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
#include "headers/level_systems.h"
#include "headers/map_generation.h"
#include "headers/render_systems.h"
#include "headers/inventory.h"
#include "headers/lua_systems.h"
#include "headers/action_systems.h"

int particleCount = 0;
RenderTileComponent **mouseEditingLayer;
/*Formula

x = (tileNum % tileSheetWidth) * 16
y = (tileNum / tileSheetHeight) * 16

*/
int renderItemIndex = 0;
RenderComponent *renderBuffer;

void SetupRenderFrame(){//Clear and allocate render buffer + reset render counter
	free(renderBuffer);
	renderItemIndex = 0;
	renderBuffer = malloc(sizeof(RenderComponent));
}

int AddToRenderQueue(SDL_Renderer *renderer, TilesheetComponent *tileSheet, int tileNum, SDL_Rect destRect, int alpha, int zPos){
	return AddToRenderQueueEx(renderer, tileSheet, tileNum, destRect, alpha, zPos, 0);
}

int AddToRenderQueueEx(SDL_Renderer *renderer, TilesheetComponent *tileSheet, int tileNum, SDL_Rect destRect, int alpha, int zPos, int rotation){
	if(alpha == -1){
		alpha = 255;
	}
	if(tileNum <= tileSheet->w * tileSheet->h - 1){
		renderBuffer = realloc(renderBuffer, (renderItemIndex + 1) * sizeof(RenderComponent));
		renderBuffer[renderItemIndex] = (RenderComponent){renderer, tileSheet, tileNum, destRect, alpha, zPos, rotation};
		renderItemIndex++;
	}else if(strcmp(tileSheet->name, "undefined") == 0 || tileSheet->tex == NULL || tileNum < 0){
		renderBuffer = realloc(renderBuffer, (renderItemIndex + 1) * sizeof(RenderComponent));
		renderBuffer[renderItemIndex] = (RenderComponent){renderer, &undefinedSheet, 0, destRect, 255, zPos, 0};
		renderItemIndex++;
	}else{
		printf("Error: Tile index not in image bounds!\n");
		return 1;
	}
	return 0;
}

// int RenderTextureFromSheet(){
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
		SDL_RenderCopyEx(renderBuffer[i].renderer, renderBuffer[i].tileSheet->tex, &sourceRect, &renderBuffer[i].transform, renderBuffer[i].rotation * 90, NULL, SDL_FLIP_NONE);	
	}
	SetupRenderFrame();
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

void RenderDroppedItems(){
	for(int i = 0; i < numDroppedItems; i++){
		SDL_Rect itemRect = {droppedItems[i].transform.worldPos.x - mapOffsetPos.x, 
			droppedItems[i].transform.worldPos.y - mapOffsetPos.y + droppedItems[i].animLocation,
			32, 32};
		SDL_Rect winRect = {-tileSize, -tileSize, WIDTH + tileSize, HEIGHT + tileSize};
		SDL_Point p = {itemRect.x, itemRect.y};
		if(SDL_PointInRect(&p, &winRect)){
			AddToRenderQueue(renderer, &droppedItems[i].item->sheet, droppedItems[i].item->tile, itemRect, 255, RNDRLYR_PLAYER - 1);
			if(droppedItems[i].animDir == 1){
				droppedItems[i].animLocation -= 0.6;
			}else if(droppedItems[i].animDir == 0){
				droppedItems[i].animLocation += 0.6;
			}
			if(droppedItems[i].animLocation >= 10){
				droppedItems[i].animDir = 1;
			}else if(droppedItems[i].animLocation <= 0){
				droppedItems[i].animDir = 0;
			}

			if(SDL_HasIntersection(&character.collider.boundingBox, &itemRect)){
				INV_WriteCell("add", INV_FindEmpty(droppedItems[i].item), droppedItems[i].qty, droppedItems[i].item);

				for(int j = i; j < numDroppedItems; j++){
					droppedItems[j] = droppedItems[j + 1];
				}

				numDroppedItems--;
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
	
	if(SDL_PointInRect((SDL_Point *) &mouseTransform.screenPos, &rect)){
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