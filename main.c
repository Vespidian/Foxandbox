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
#include "headers/renderSystems.h"
#include "headers/data.h"
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
void RenderScreen();

//MISC
bool quit = false;
static const int targetFramerate = 60;
float deltaTime = 0;
float loopStartTime = 0;
/*VARIABLE DECLARATION*/
int tileSize = 64;

Vector2 mapOffsetPos = {0, 0};//Offset of the map to simulate movement
Vector2 playerCoord = {0, 0};//Player's coordinate on map
Vector2 placeLocation = {0, 0};
Vector2 characterOffset = {0, 0};//Position of character sprite relative to window 0,0
Vector2 mousePos = {0, 0};//Position of the mouse
Vector2 midScreen = {0, 0};

bool enableHitboxes = false;

bool isWalking = false;
int characterFacing = 0;
bool uiInteractMode = false;

SDL_Rect windowRect;

int darknessMod = 0;
bool isDay = true;
bool doDayCycle = false;
float playerSpeed = 2;

bool mouseClicked = false;

ParticleSystem pSys1;


void clearScreen(SDL_Renderer *renderer){
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
	SDL_RenderClear(renderer);
}

void DrawAnimation(SDL_Rect dest, WB_Tilesheet tileSheet, int startFrame, int numFrames, int delay){
	int frame = 0;
	frame = (SDL_GetTicks() / delay) % numFrames;
	
	AddToRenderQueue(gRenderer, tileSheet, startFrame + frame, dest, -1, RNDRLYR_PLAYER);
}

void DrawCharacter(int direction, int numFrames){
	SDL_Rect charPos = {characterOffset.x, characterOffset.y, tileSize, tileSize};
	
	if(isWalking){
		DrawAnimation(charPos, characterSheet, (direction) * 6, numFrames, 100);
	}else{
		DrawAnimation(charPos, characterSheet, (direction + 2) * 6, numFrames, 100);
	}
}

void Setup(){
	SDL_Rect windowRect = {-tileSize, -tileSize, WIDTH + tileSize, HEIGHT + tileSize};
	
	SDL_GetWindowSize(gWindow, &WIDTH, &HEIGHT);
	midScreen.x = (WIDTH / 2 - tileSize / 2);
	midScreen.y = (HEIGHT / 2 - tileSize / 2);
	characterOffset = midScreen;
	SetupRenderFrame();
	GenerateProceduralMap(50, 5);
	
	SDL_RenderCopy(gRenderer, backgroundTex, NULL, NULL);
	SDL_RenderPresent(gRenderer);
	NewParticleSystem(&pSys1, 1, (SDL_Rect){0, 0, WIDTH, HEIGHT}, 1000, (Range)/*x*/{-1, 1}, (Range)/*y*/{1, 1}, (Range){20, 70});//Snow
	// NewParticleSystem(&pSys1, 2, (SDL_Rect){0, 0, WIDTH, HEIGHT}, 100, (Range)/*x*/{0, 0}, (Range)/*y*/{5, 6}, (Range){20, 70});//Rain
	pSys1.boundaryCheck = true;
	// pSys1.fade = false;
	pSys1.playSystem = false;
	
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
			loopStartTime = SDL_GetTicks();
		
			if((doDayCycle && SDL_GetTicks() / 10) % 20 == 1){
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
			
			
			const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
			if(currentKeyStates[SDL_SCANCODE_RETURN] || currentKeyStates[SDL_SCANCODE_ESCAPE]){
				quit = true;
			}
			
			isWalking = false;
			if(currentKeyStates[SDL_SCANCODE_A]){
				if(!colLeft){
					mapOffsetPos.x -= playerSpeed * deltaTime;
					isWalking = true;
				}
				characterFacing = 0;
			}
			if(currentKeyStates[SDL_SCANCODE_W]){
				if(!colUp){
					mapOffsetPos.y -= playerSpeed * deltaTime;
					isWalking = true;
				}
			}
			if(currentKeyStates[SDL_SCANCODE_D]){
				if(!colRight){
					mapOffsetPos.x += playerSpeed * deltaTime;
					isWalking = true;
				}
				characterFacing = 1;
			}
			if(currentKeyStates[SDL_SCANCODE_S]){
				if(!colDown){
					mapOffsetPos.y += playerSpeed * deltaTime;
					isWalking = true;
				}
			}
			if(currentKeyStates[SDL_SCANCODE_Q]){
				characterOffset.x = 0;
				characterOffset.y = 0;
				printf("Character Offset Reset!");
			}
			
			mouseClicked = false;
			while(SDL_PollEvent(&e) != 0){
				SDL_GetMouseState(&mousePos.x, &mousePos.y);
				
				if(e.type == SDL_MOUSEBUTTONDOWN){
					if(e.key.state == SDL_RELEASED){
						printf("boop\n");
						mouseClicked = true;
					}
				}
				
				/*if(e.type == SDL_MOUSEBUTTONUP){
					// if(e.key.state = SDL_PRESSED){
						printf("boop\n");
						mouseClicked = false;
					// }
				}*/
				
				if(e.type == SDL_KEYDOWN){
					if(e.key.keysym.sym == SDLK_y){
						characterOffset.x = midScreen.x;
						characterOffset.y = midScreen.y;
					}
				}
				if(e.type == SDL_KEYUP){
					Vector2 roundSpeed = {mapOffsetPos.x % 4, mapOffsetPos.y % 4};//Make sure the character position is always a multiple of 4\
					keeping everything pixel perfect
					if(roundSpeed.x != 0){
						mapOffsetPos.x -= roundSpeed.x;
					}
					if(roundSpeed.y != 0){
						mapOffsetPos.y -= roundSpeed.y;
					}
					
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
						/*if(INV_FindItem(1) != -1 && customMap[playerCoord.y + placeLocation.y][playerCoord.x  + placeLocation.x].type != 49){
							customMap[playerCoord.y + placeLocation.y][playerCoord.x + placeLocation.x].type = 49;
							INV_WriteCell("sub", INV_FindItem(1), 1, 1);
						}*/
						buildLayer_tmp[playerCoord.y + placeLocation.y][playerCoord.x + placeLocation.x].type = 0;
						// AutotileMap();
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

			RenderScreen();
			SDL_Delay(1000 / targetFramerate);//Game FrameRate
			deltaTime = (SDL_GetTicks() - loopStartTime) / 10;
			// char *deltaTemp;
			// snprintf(deltaTemp, 1024, "Delta Time: %f", deltaTime);
			// RenderText_d(gRenderer, deltaTemp, 0, 300);
		}
	}else{
		printf("Failed to initialize\n");
	}
	
	if(quit){
		Quit();
	}
}

/*typedef struct{
	bool mouseDown;
	bool mouseUp;
	
	bool prevMouseState;
	bool mouseClicked
}MouseStates;

MouseStates mouse;*/


void RenderScreen(){
	clearScreen(gRenderer);
	
	//Call SDL draw functions here and call RenderScreen from the main loop
	
	DrawLevel();
	
	//Render the player's hitbox
	if(enableHitboxes){
		AddToRenderQueue(gRenderer, uiSheet, 1, charCollider_left, -1, 750);
		AddToRenderQueue(gRenderer, uiSheet, 1, charCollider_right, -1, 750);
		AddToRenderQueue(gRenderer, uiSheet, 1, charCollider_bottom, -1, 750);
		AddToRenderQueue(gRenderer, uiSheet, 1, charCollider_top, -1, 750);
	}
	
	FindCollisions();
	
	INV_DrawInv();

	char coordinates[256];
	char charoff[256];
	
	playerCoord = (Vector2){
		((characterOffset.x + 32) + mapOffsetPos.x) / 64,
		((characterOffset.y + 32) + mapOffsetPos.y) / 64,
	};
	
	SDL_Rect mapRect = {-mapOffsetPos.x + 4, -mapOffsetPos.y + 4, 64 * 32 - 8, 64 * 32 - 8};
	pSys1.area = mapRect;
	RenderParticleSystem(pSys1);
	
	
	snprintf(coordinates, 1024, "Player Coordinates ->\nx: %d, y: %d", playerCoord.x, playerCoord.y);
	snprintf(charoff, 1024, "MapOffset ->\nx: %d, y: %d", mapOffsetPos.x, mapOffsetPos.y);
	
	RenderText_d(gRenderer, coordinates, 0, 0);
	RenderText_d(gRenderer, charoff, 0, 48);	
	
	SDL_Rect woahR = {0, 0, 100, 100};
	AddToRenderQueue(gRenderer, colorModSheet, 0, woahR, 0, 1000);
	RenderCursor();
	
	
	char frameCount[64];
	snprintf(frameCount, 128, "%d RenderCopy calls", renderItemIndex);
	RenderText_d(gRenderer, frameCount, WIDTH - 200, 0);

	
	RenderUpdate();
	particleCount = 0;
	
	SDL_RenderCopy(gRenderer, colorModTex, NULL, NULL);
	SDL_RenderPresent(gRenderer);
}


void RenderTextureInWorld(SDL_Renderer *renderer, WB_Tilesheet sheet, int tile, SDL_Rect rect, int zPos){
	rect.x -= mapOffsetPos.x;
	rect.y -= mapOffsetPos.y;
	AddToRenderQueue(renderer, sheet, tile, rect, -1, zPos);
}


/*
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
}*/
