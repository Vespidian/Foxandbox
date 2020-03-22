#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include "headers/DataTypes.h"
#include "headers/initialize.h"
#include "headers/data.h"
#include "headers/drawFunctions.h"
#include "headers/tileMap.h"
#include "headers/collision.h"
// Normally SDL2 will redefine the main entry point of the program for Windows applications
// this doesn't seem to play nice with TCC, so we just undefine the redefinition
#ifdef __TINYC__
    #undef main
#endif

//FUNCTION PREDEFINITIONS
bool init();
void RenderScreen();

SDL_Event e;

void clearScreen(){
	SDL_SetRenderDrawColor(gRenderer, 10, 10, 10, 0xff);
	SDL_RenderClear(gRenderer);
}
//MISC
bool quit = false;
static const int targetFramerate = 30;

/*VARIABLE DECLARATION*/
int tileSize = 64;

Vector2 worldPosition = {0, 0};
Vector2 characterOffset = {0, 0};
Vector2 mousePos = {0, 0};
Vector2 midScreen = {0, 0};

int layerOrder = 0;
// bool isBehind = false;
bool enableHitboxes = false;

int characterFacing = 0;

void DrawCharacter(int direction, int numFrames){
	SDL_Rect charPos = {characterOffset.x, characterOffset.y, tileSize, tileSize};
	
	int animFrame = 0;
	
	animFrame = (int)(SDL_GetTicks() / 300) % numFrames;
	
	RenderTextureFromSheet(gRenderer, characterTex, 4, 6, 16, animFrame + (direction * 4), charPos);
}

int main(int argc, char **argv) {
	init();
	if(init){
		SDL_GetWindowSize(gWindow, &WIDTH, &HEIGHT);
		midScreen.x = (WIDTH / 2 - tileSize / 2);
		midScreen.y = (HEIGHT / 2 - tileSize / 2);
		characterOffset = midScreen;
		while(!quit){	
			
			RenderScreen();
			
			
			const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
			if(currentKeyStates[SDL_SCANCODE_RETURN] || currentKeyStates[SDL_SCANCODE_ESCAPE]){
				quit = true;
			}

			characterFacing = 0;
			if(currentKeyStates[SDL_SCANCODE_A]){
				if(!colLeft){
					worldPosition.x -= 4;
				}
				characterFacing = 4;
			}
			if(currentKeyStates[SDL_SCANCODE_W]){
				if(!colUp){
					worldPosition.y -= 4;
				}
				characterFacing = 3;
			}
			if(currentKeyStates[SDL_SCANCODE_D]){
				if(!colRight){
					worldPosition.x += 4;
				}
				characterFacing = 2;
			}
			if(currentKeyStates[SDL_SCANCODE_S]){
				if(!colDown){
					worldPosition.y += 4;
				}
				characterFacing = 1;
			}
			
			if(currentKeyStates[SDL_SCANCODE_E]){
				printf("%d, %d\n", worldPosition.x * 4, worldPosition.y * 4);
			}
			if(currentKeyStates[SDL_SCANCODE_C]){
				printf("%d, %d\n", characterOffset.x, characterOffset.y);
			}
			if(currentKeyStates[SDL_SCANCODE_R]){
				printf("Width: %d, Height: %d\n", WIDTH, HEIGHT);
			}
			if(currentKeyStates[SDL_SCANCODE_Q]){
				// printf("%d, %d\n", characterOffset.x, characterOffset.y);
				characterOffset.x = 0;
				characterOffset.y = 0;
				printf("Character Offset Reset!");
			}
			
			while(SDL_PollEvent(&e) != 0){				
				if(e.type == SDL_KEYDOWN){
					if(e.key.keysym.sym == SDLK_t){
						TextureDestroy();
						TextureInit();
						MapInit();
					}
					if(e.key.keysym.sym == SDLK_y){
						characterOffset.x = midScreen.x;
						characterOffset.y = midScreen.y;
					}
					if(e.key.keysym.sym == SDLK_x){
						if(!enableHitboxes){
							enableHitboxes = true;
						}else{
							enableHitboxes = false;
						}
					}
				}else if(e.type == SDL_QUIT){
					quit = true;
				}
			}
			//Game FrameRate
			SDL_Delay(1000 / targetFramerate);
		}
	}else{
		printf("Failed to initialize\n");
	}
	
	if(quit){
		Quit();
	}
}

void RenderScreen(){
	clearScreen();
	
	SDL_RenderCopy(gRenderer, backgroundTex, NULL, NULL);
	//Call SDL draw functions here and call RenderScreen from the main loop
	DrawMap(tileSheetTex, 16, map);
	DrawMap(tileSheetTex, 16, map1);
	// RenderText("woohoo", 100, 100);
	if(layerOrder == 0){
		DrawMap(furnitureTex, 8, furnitureMap);
		DrawMap(furnitureTex, 8, passableMap);
		DrawCharacter(characterFacing, 2);
		
	}else if(layerOrder == 1){
		DrawMap(furnitureTex, 8, passableMap);
		DrawCharacter(characterFacing, 2);
		DrawMap(furnitureTex, 8, furnitureMap);
		
	}else if(layerOrder == 2){
		DrawMap(furnitureTex, 8, furnitureMap);
		DrawCharacter(characterFacing, 2);
		DrawMap(furnitureTex, 8, passableMap);
		
	}else if(layerOrder == 3){		
		DrawCharacter(characterFacing, 2);
		DrawMap(furnitureTex, 8, passableMap);
		DrawMap(furnitureTex, 8, furnitureMap);
		
	}else if(layerOrder == 4){		
		DrawCharacter(characterFacing, 2);
		DrawMap(furnitureTex, 8, furnitureMap);
		DrawMap(furnitureTex, 8, passableMap);
		
	}
	
	SDL_RenderCopy(gRenderer, colorModTex, NULL, NULL);
	//Render the player's hitbox
	SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, 0);
	if(enableHitboxes){
		SDL_RenderDrawRect(gRenderer, &charCollider_top);
		SDL_RenderDrawRect(gRenderer, &charCollider_bottom);
		SDL_RenderDrawRect(gRenderer, &charCollider_right);
		SDL_RenderDrawRect(gRenderer, &charCollider_left);
	}
	
	FindCollisions();
	
	SDL_RenderPresent(gRenderer);
}

/*void RenderText(char *text, int x, int y){
	SDL_Color coloor = {255, 255, 255};
	SDL_Surface *fSurface = TTF_RenderText_Blended(font, text, coloor);//problem here
	SDL_Texture *fTexture = SDL_CreateTextureFromSurface(gRenderer, fSurface);
	
	SDL_Rect destRect = {x, y, 500, 500};
	
	// SDL_Surface *screen = SDL_GetWindowSurface(gWindow);
	SDL_QueryTexture(fTexture, NULL, NULL, &destRect.w, &destRect.h);
	SDL_RenderCopy(gRenderer, fTexture, NULL, &destRect);
	// SDL_BlitSurface(fSurface, NULL, screen, &destRect);
	
	SDL_DestroyTexture(fTexture);
	// SDL_FreeSurface(screen);
	SDL_FreeSurface(fSurface);
}*/