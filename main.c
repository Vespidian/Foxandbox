#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL_image.h>

#include "headers/DataTypes.h"
#include "headers/ECS.h"
#include "headers/initialize.h"
#include "headers/data.h"
#include "headers/drawFunctions.h"
#include "headers/tileMap.h"
#include "headers/collision.h"
#include "headers/inventory.h"
#include "headers/mapGeneration.h"
// Normally SDL2 will redefine the main entry point of the program for Windows applications
// this doesn't seem to play nice with TCC, so we just undefine the redefinition
#ifdef __TINYC__
    #undef main
#endif

//FUNCTION PREDEFINITIONS
bool init();
void RenderScreen();

void clearScreen(SDL_Renderer *renderer){
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
	SDL_RenderClear(renderer);
}
//MISC
bool quit = false;
static const int targetFramerate = 60;

/*VARIABLE DECLARATION*/
int tileSize = 64;

Vector2 mapOffsetPos = {0, 0};
Vector2 characterOffset = {0, 0};
Vector2 mousePos = {0, 0};
Vector2 midScreen = {0, 0};

int layerOrder = 0;
// bool isBehind = false;
bool enableHitboxes = false;

bool isWalking = false;
int characterFacing = 0;
bool uiMode = false;

void DrawAnimation(SDL_Rect dest, WB_Tilesheet tileSheet, int startFrame, int numFrames, int delay){
	int frame = 0;
	frame = (SDL_GetTicks() / delay) % numFrames;
	
	AddToRenderQueue(gRenderer, tileSheet, startFrame + frame, dest, RNDRLYR_PLAYER);
}

void DrawCharacter(int direction, int numFrames){
	SDL_Rect charPos = {characterOffset.x, characterOffset.y, tileSize, tileSize};
	
	if(isWalking){
		DrawAnimation(charPos, characterSheet, (direction) * 6, numFrames, 100);
	}else{
		DrawAnimation(charPos, characterSheet, (direction + 2) * 6, numFrames, 100);
	}
}

SDL_Rect windowRect;

ParticleSystem pSys1;

int darknessMod = 0;
bool isDay = true;
bool doDayCycle = false;
Vector2 worldPos = {0, 0};
int playerSpeed = 3;
Vector2 placeLocation = {0, 0};

void Setup(){
	// randomArray();
	SDL_Rect windowRect = {-tileSize, -tileSize, WIDTH + tileSize, HEIGHT + tileSize};
	
	SDL_GetWindowSize(gWindow, &WIDTH, &HEIGHT);
	midScreen.x = (WIDTH / 2 - tileSize / 2);
	midScreen.y = (HEIGHT / 2 - tileSize / 2);
	characterOffset = midScreen;
	NewEntity();
	SetupRenderFrame();
	GenerateProceduralMap(50, 5);
	
	NewParticleSystem(&pSys1, 1, (SDL_Rect){0, 0, WIDTH, HEIGHT}, 1000, (Range)/*x*/{-1, 1}, (Range)/*y*/{1, 1}, (Range){20, 70});
	// pSys1.fade = false;
	
}
Vector2 tmpSize;	
void ResizeWindow(){
	Vector2 roundSpeed = {mapOffsetPos.x % 4, mapOffsetPos.y % 4};//Make sure the character position is always a multiple of 4\
	keeping everything pixel perfect
	if(roundSpeed.x != 0){
		mapOffsetPos.x -= roundSpeed.x;
	}
	if(roundSpeed.y != 0){
		mapOffsetPos.y -= roundSpeed.y;
	}

	clearScreen(gRenderer);
	SDL_GetWindowSize(gWindow, &WIDTH, &HEIGHT);
	Vector2 diff = {WIDTH - tmpSize.x, HEIGHT - tmpSize.y};
	
	mapOffsetPos.x -= diff.x / 2;
	mapOffsetPos.y -= diff.y / 2;
	
	characterOffset.x = WIDTH / 2 - tileSize / 2;
	characterOffset.y = HEIGHT / 2 - tileSize / 2;
	
	tmpSize = (Vector2){WIDTH, HEIGHT};
	windowRect = (SDL_Rect){-tileSize, -tileSize, WIDTH + tileSize, HEIGHT + tileSize};
}

int main(int argc, char **argv) {
	init();
	if(init){
		Setup();
		tmpSize = (Vector2){WIDTH, HEIGHT};
		while(!quit){	
			// int time = SDL_GetTicks();
			// float timeDiff;
		
			if((SDL_GetTicks() / 10) % 20 == 1 && doDayCycle){
				if(darknessMod == 0){
					isDay = true;
				}
				if(darknessMod == 200){
					isDay = false;
				}
				if(isDay){
					darknessMod++;
				}else{
					darknessMod--;
				}
				// printf("%d\n", darknessMod);
				SDL_SetTextureAlphaMod(colorModTex, darknessMod);
			}
			
			if(characterFacing == 4){
				placeLocation = (Vector2){-1, 0};
			}else if(characterFacing == 3){
				placeLocation = (Vector2){1, 0};
			}else if(characterFacing == 2){
				placeLocation = (Vector2){0, -1};
			}else if(characterFacing == 1){
				placeLocation = (Vector2){0, 1};
			}
			
			RenderScreen();
			
			const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
			if(currentKeyStates[SDL_SCANCODE_RETURN] || currentKeyStates[SDL_SCANCODE_ESCAPE]){
				quit = true;
			}

			// characterFacing = 0;
			isWalking = false;
			
			
			if(currentKeyStates[SDL_SCANCODE_A]){
				if(!colLeft){
					// if(SDL_GetTicks() % 2 != 0)
					mapOffsetPos.x -= playerSpeed;
					isWalking = true;
				}
				characterFacing = 0;
			}
			if(currentKeyStates[SDL_SCANCODE_W]){
				if(!colUp){
					// if(SDL_GetTicks() % 2 == 0)
					mapOffsetPos.y -= playerSpeed;
					isWalking = true;
				}
				// characterFacing = 0;
				// worldPos.y -= 1;
			}
			if(currentKeyStates[SDL_SCANCODE_D]){
				if(!colRight){
					// if(SDL_GetTicks() % 2 == 0)
					mapOffsetPos.x += playerSpeed;
					isWalking = true;
				}
				characterFacing = 1;
				// worldPos.x += 1;
			}
			if(currentKeyStates[SDL_SCANCODE_S]){
				if(!colDown){
					// if(SDL_GetTicks() % 2 == 0)
					mapOffsetPos.y += playerSpeed;
					isWalking = true;
				}
				// characterFacing = 1;
				// worldPos.y += 1;
			}
				// worldPos.y += characterFacing % 4;
			
			// if(currentKeyStates[SDL_SCANCODE_E]){
				// printf("%d, %d\n", mapOffsetPos.x * 4, mapOffsetPos.y * 4);
			// }
			if(currentKeyStates[SDL_SCANCODE_Q]){
				// printf("%d, %d\n", characterOffset.x, characterOffset.y);
				characterOffset.x = 0;
				characterOffset.y = 0;
				printf("Character Offset Reset!");
			}
			
			while(SDL_PollEvent(&e) != 0){				
				if(e.type == SDL_KEYDOWN){
					if(e.key.keysym.sym == SDLK_y){
						characterOffset.x = midScreen.x;
						characterOffset.y = midScreen.y;
					}
				}
				if(e.type == SDL_KEYUP){
					
					/*Vector2 roundSpeed = {mapOffsetPos.x % 4, mapOffsetPos.y % 4};//Make sure the character position is always a multiple of 4\
					keeping everything pixel perfect
					if(roundSpeed.x != 0){
						mapOffsetPos.x += roundSpeed.x;
					}
					if(roundSpeed.y != 0){
						mapOffsetPos.y += roundSpeed.y;
					}*/
					
					if(e.key.keysym.sym == SDLK_c){
						noClip = !noClip;
					}
					if(e.key.keysym.sym == SDLK_x){
						enableHitboxes = !enableHitboxes;
					}
					if(e.key.keysym.sym == SDLK_e){
						showInv = !showInv;
					}
					if(e.key.keysym.sym == SDLK_v){
						/*if(INV_FindItem(1) != -1 && customMap[worldPos.y + placeLocation.y][worldPos.x  + placeLocation.x].type != 49){
							customMap[worldPos.y + placeLocation.y][worldPos.x + placeLocation.x].type = 49;
							INV_WriteCell("sub", INV_FindItem(1), 1, 1);
						}*/
						tempArray[worldPos.y + placeLocation.y][worldPos.x + placeLocation.x].type = 0;
						// DefineBorder();
					}
					if(e.key.keysym.sym == SDLK_r){
						INV_WriteCell("add", INV_FindEmpty(0), 10, 1);
						// TextExtrapolate(colMap);
					}
					if(e.key.keysym.sym == SDLK_b){
						SDL_DisplayMode gMode;
						SDL_GetDesktopDisplayMode(0, &gMode);
						WIDTH = gMode.w;
						HEIGHT = gMode.h;
						SDL_SetWindowBordered(gWindow, false);
						SDL_SetWindowPosition(gWindow, 0, 0);
						SDL_SetWindowSize(gWindow, WIDTH, HEIGHT);
						ResizeWindow();
					}
				}
				if(e.type == SDL_MOUSEWHEEL){
					if(e.wheel.y > 0){
						if(selectedHotbar <= INV_WIDTH && selectedHotbar > 0){
							selectedHotbar--;
						}else{
							selectedHotbar = INV_WIDTH - 1;
						}
					}else if(e.wheel.y < 0){
						if(selectedHotbar < INV_WIDTH - 1 && selectedHotbar >= 0){
							selectedHotbar++;
						}else{
							selectedHotbar = 0;
						}
					}
				}
				if(e.type == SDL_WINDOWEVENT){
					if(e.window.event == SDL_WINDOWEVENT_RESIZED){
						ResizeWindow();
					}
				}
				
				if(e.type == SDL_QUIT){
					quit = true;
				}
			}
			//Game FrameRate
			// timeDiff = SDL_GetTicks() - time;
			SDL_Delay(1000 / targetFramerate);
		}
	}else{
		printf("Failed to initialize\n");
	}
	
	if(quit){
		Quit();
	}
}
	Vector2 tempEntity = {0, 0};

void RenderScreen(){
	clearScreen(gRenderer);
	
	SDL_RenderCopy(gRenderer, backgroundTex, NULL, NULL);
	//Call SDL draw functions here and call RenderScreen from the main loop
	
	DrawLevel();
	// DrawCharacter(characterFacing, 4);
	
	//Render the player's hitbox
	// SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, 0);
	if(enableHitboxes){
		AddToRenderQueue(gRenderer, uiSheet, 1, charCollider_left, 750);
		AddToRenderQueue(gRenderer, uiSheet, 1, charCollider_right, 750);
		AddToRenderQueue(gRenderer, uiSheet, 1, charCollider_bottom, 750);
		AddToRenderQueue(gRenderer, uiSheet, 1, charCollider_top, 750);
	}
	
	FindCollisions();
	
	INV_DrawInv();

	char coordinates[256];
	char charoff[256];
	
	worldPos = (Vector2){
		((characterOffset.x + 32) + mapOffsetPos.x) / 64,
		((characterOffset.y + 32) + mapOffsetPos.y) / 64,
	};
	
	
	RenderParticleSystem(pSys1);

	
	/*if(tempEntity.x > mapOffsetPos.x + characterOffset.x){
		tempEntity.x--;
	}else if(tempEntity.x < mapOffsetPos.x + characterOffset.x){
		tempEntity.x++;
	}
	if(tempEntity.y > mapOffsetPos.y + characterOffset.y){
		tempEntity.y--;
	}else if(tempEntity.y < mapOffsetPos.y + characterOffset.y){
		tempEntity.y++;
	}
	// SDL_Rect entityRect = {tempEntity.x - mapOffsetPos.x, tempEntity.y - mapOffsetPos.y, 64, 64};
	SDL_Rect entityRect = {tempEntity.x - mapOffsetPos.x, tempEntity.y - mapOffsetPos.y, 64, 64};
	// RenderTextureFromSheet(gRenderer, furnitureSheet, 1, entityRect);
	// RenderTextureInWorld(gRenderer, furnitureSheet, 1, entityRect, RNDRLYR_PLAYER - 1);
	DrawAnimation(entityRect, zombieSheet, 0, 4, 200);*/
	
	
	// SDL_Rect showPointRect = {(worldPos.x * 64 - mapOffsetPos.x) + placeLocation.x, (worldPos.y * 64 - mapOffsetPos.y) + placeLocation.y, 64, 64};
	// SDL_RenderDrawRect(gRenderer, &showPointRect);
	
	// snprintf(coordinates, 1024, "x: %d, y: %d", (mapOffsetPos.x + 32) / 64, (mapOffsetPos.y + 32) / 64);
	snprintf(coordinates, 1024, "Player Coordinates ->\nx: %d, y: %d", worldPos.x, worldPos.y);
	snprintf(charoff, 1024, "MapOffset ->\nx: %d, y: %d", mapOffsetPos.x, mapOffsetPos.y);
	
	RenderText_d(gRenderer, coordinates, 0, 0);
	RenderText_d(gRenderer, charoff, 0, 48);	
	
	SDL_Rect woahR = {0, 0, 100, 100};
	AddToRenderQueue(gRenderer, colorModSheet, 0, woahR, 1000);
	RenderCursor();
	
	RenderEntities();
	RenderUpdate();
	
	SDL_RenderCopy(gRenderer, colorModTex, NULL, NULL);
	SDL_RenderPresent(gRenderer);
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
				AddToRenderQueue(renderer, fontSheet, charVal, charRect, RNDRLYR_TEXT);
				charRect.x += tracking;
			}else if(charVal == -22){//NEWLINE (\n)
				charRect.y += spacing;
				charRect.x = x;
			}else if(charVal == -23){//TAB
				charRect.x += tracking * 4;
			}
		}
	}else if(strlen(text) < 1){
		printf("Error: No text provided on RenderText()\n");
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
	SDL_GetMouseState(&mousePos.x, &mousePos.y);
	Vector2 mouseTile = {mousePos.x, mousePos.y};
	mouseTile.x = ((mousePos.x + mapOffsetPos.x) / 64);
	mouseTile.y = ((mousePos.y + mapOffsetPos.y) / 64);
	SDL_Point cursor = {(mouseTile.x * 64) - mapOffsetPos.x, (mouseTile.y * 64) - mapOffsetPos.y};
	if(SDL_PointInRect(&cursor, &mapRect) && !uiMode){
		AddToRenderQueue(gRenderer, debugSheet, 0, (SDL_Rect){cursor.x, cursor.y, 64, 64}, RNDRLYR_UI - 1);
		
		{//MouseText
			char mousePosT[256];
			snprintf(mousePosT, 1024, "MOUSEPOS ->\nx: %d, y: %d", mouseTile.x, mouseTile.y);
			RenderText_d(gRenderer, mousePosT, 0, 96);
		}
		
		
		if(e.type == SDL_MOUSEBUTTONDOWN){//Place and remove tiles
			if(e.button.button == SDL_BUTTON_LEFT){
				TileMapEdit(tempArray, (Vector2){mouseTile.x, mouseTile.y}, 0, false);
				TileMapEdit(randArray, (Vector2){mouseTile.x, mouseTile.y}, 0, false);
				DefineBorder(randArray);
			}else if(e.button.button == SDL_BUTTON_RIGHT){
				TileMapEdit(randArray, (Vector2){mouseTile.x, mouseTile.y}, 47, false);
				TileMapEdit(tempArray, (Vector2){mouseTile.x, mouseTile.y}, 47, false);
				DefineBorder(randArray);
			}
		}
	}
}



Vector2 ConvertToWorldPos(Vector2 pos){
	return (Vector2){pos.x - mapOffsetPos.x, pos.y - mapOffsetPos.y};
}

void RenderTextureInWorld(SDL_Renderer *renderer, WB_Tilesheet sheet, int tile, SDL_Rect rect, int zPos){
	rect.x -= mapOffsetPos.x;
	rect.y -= mapOffsetPos.y;
	AddToRenderQueue(renderer, sheet, tile, rect, zPos);
}




/*
Need to:
- Write a function the returns an SDL_Rect from a WB_Tilesheet input and a number int
- 
*/

void SpawnParticle(ParticleComponent *particle, int pType, SDL_Rect spawnArea, Range xR, Range yR, Range duration){
	particle->active = true;//Activate the particle
	// particle->duration = getRnd(20, 70);
	particle->type = pType;
	particle->initDuration = getRnd(duration.min, duration.max);
	particle->duration = particle->initDuration;
	particle->size = 4;
	particle->pos.x = getRnd(spawnArea.x, spawnArea.x + spawnArea.w);
	particle->pos.y = getRnd(spawnArea.y, spawnArea.y + spawnArea.h);
	particle->pos.w = particle->size;
	particle->pos.h = particle->size;
	
	particle->dir.x = getRnd(xR.min, xR.max + 1);
	particle->dir.y = getRnd(yR.min, yR.max + 1);
	
	// printf("%d\n", particle.pos.w);
}

void NewParticleSystem(ParticleSystem *pSystem, int pType, SDL_Rect area, int particleNum, Range xR, Range yR, Range duration){
	pSystem->playSystem = true;
	pSystem->pType = pType;
		// printf("%d", testingParticles.playSystem);
	// pSystem.area = (SDL_Rect){200, 200, 16, 16};
	// pSystem.area = (SDL_Rect){0, 0, WIDTH, HEIGHT};
	pSystem->area = (SDL_Rect)area;
	pSystem->fade = true;
	// pSystem.maxParticles = 25;
	pSystem->maxParticles = particleNum;
	// pSystem->sysDir = sysDir;
	pSystem->xR = xR;
	pSystem->yR = yR;
	pSystem->duration = duration;
	
	pSystem->particles = malloc(sizeof(ParticleComponent));
	
	for(int i = 0; i < pSystem->maxParticles; i++){	
		pSystem->particles = realloc(pSystem->particles, (i + 1) * sizeof(ParticleComponent));//Allocate the space for the particle
		
		pSystem->particles[i].pTex = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1, 1);
		SDL_SetTextureBlendMode(pSystem->particles[i].pTex, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(gRenderer, pSystem->particles[i].pTex);

		//Implement using the particle->type as the texture location
		SDL_Rect sourceRect = {(pType % 4) * 1, (pType / 4) * 1, 1, 1};
		// SDL_Rect sourceRect = {(pType % 4) * 4, (pType / 4) * 4, 4, 4};
		SDL_Rect destRect = {0, 0, 1, 1};
		SDL_RenderCopy(gRenderer, particleSheet.tex, &sourceRect, &destRect);
		
		SDL_SetRenderTarget(gRenderer, NULL);
		
		// SpawnParticle(&pSystem->particles[i], pSystem->area, pSystem->xR, pSystem->yR, pSystem->duration);
		SpawnParticle(&pSystem->particles[i], pType, area, xR, yR, duration);
		// printf("%d\n", i);
	}
}

void RenderParticleSystem(ParticleSystem system){
	if(system.playSystem){
		// system.area = (SDL_Rect){mousePos.x, mousePos.y, 16, 16};
		for(int i = 0; i < system.maxParticles; i++){
			ParticleComponent particle = (ParticleComponent)system.particles[i];
			if(particle.active == true){
				particle.pos.x += particle.dir.x;
				particle.pos.y += particle.dir.y;
				particle.duration -= 1;
				if(system.fade == true){
					SDL_SetTextureAlphaMod(particle.pTex, (particle.duration * 255) / particle.initDuration);
				}
				if(particle.duration <= 0){//Reset the particle
					particle.duration = 0;
					particle.active = false;
				}
				
				// AddToRenderQueue_custom(gRenderer, particle.pTex, 4, particle.pos, RNDRLYR_UI - 5);
				AddToRenderQueue_custom(gRenderer, particle.pTex, 1, particle.pos, RNDRLYR_UI - 5);
				// AddToRenderQueue(gRenderer, particleSheet, 0, particle.pos, RNDRLYR_UI - 5);
				// printf(".");
				system.particles[i] = (ParticleComponent)particle;
			}else{
				SpawnParticle(&system.particles[i], system.pType, system.area, system.xR, system.yR, system.duration);
			}
		}
	}
}


Entity *enemies;
void NewEntity(){
	enemies = (Entity *) malloc(sizeof(Entity));
	//Declare renderer component
	SDL_Rect enemyPos = {50, 50, 64, 64};
	enemies[0].renderer.renderer = gRenderer;
	enemies[0].renderer.tileSheet = itemSheet;
	enemies[0].renderer.tile = 24;
	enemies[0].renderer.transform = enemyPos;
}

void RenderEntities(){
	for(int i = 0; sizeof(enemies) / sizeof(enemies[0]); i++){
		RenderTextureInWorld(enemies[i].renderer.renderer, enemies[i].renderer.tileSheet, enemies[i].renderer.tile, enemies[i].renderer.transform, 0);
	}
}
