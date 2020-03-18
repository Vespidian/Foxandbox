#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>

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
Vector2 mousePos = {0, 0};

int layerOrder = 0;
// bool isBehind = false;
bool enableHitboxes = false;


int characterFacing = 0;

void DrawCharacter(int direction){
	SDL_Rect charPos = {(WIDTH / 2 - tileSize / 2), (HEIGHT / 2 - tileSize / 2), tileSize, tileSize};
	
	if(direction == 2){
		RenderTextureFromSheet(gRenderer, characterTex, 4, 16, 0, charPos);
		
	}else if(direction == 0){
		RenderTextureFromSheet(gRenderer, characterTex, 4, 16, 1, charPos);
		
	}else if(direction == 1){
		RenderTextureFromSheet(gRenderer, characterTex, 4, 16, 2, charPos);
		
	}else if(direction == 3){
		RenderTextureFromSheet(gRenderer, characterTex, 4, 16, 3, charPos);
		
	}
}

int main(int argc, char **argv) {
	init();
	
	if(init){
		while(!quit){	
			
			RenderScreen();
			
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
			const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
			if(currentKeyStates[SDL_SCANCODE_RETURN] || currentKeyStates[SDL_SCANCODE_ESCAPE]){
				quit = true;
			}

			if(currentKeyStates[SDL_SCANCODE_W]){
				if(!colUp){
					worldPosition.y += 4;
				}
				characterFacing = 2;
			}
			if(currentKeyStates[SDL_SCANCODE_A]){
				if(!colLeft){
					worldPosition.x += 4;	
				}
				characterFacing = 1;
			}
			if(currentKeyStates[SDL_SCANCODE_D]){
				if(!colRight){
					worldPosition.x -= 4;
				}
				characterFacing = 3;
			}
			if(currentKeyStates[SDL_SCANCODE_S]){
				if(!colDown){
					worldPosition.y -= 4;
				}
				characterFacing = 0;
			}
			
			if(currentKeyStates[SDL_SCANCODE_E]){
				map[5][4] = 7;
				printf("%d, %d\n", -worldPosition.x, -worldPosition.y + HEIGHT/2);
			}
			
			
			while(SDL_PollEvent(&e) != 0){				
				if(e.type == SDL_KEYDOWN){
					if(e.key.keysym.sym == SDLK_t){
						TextureDestroy();
						TextureInit();
						MapInit();
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
	
	//Call SDL draw functions here and call RenderScreen from the main loop
	DrawMap(tileSheetTex, 16, map);
	DrawMap(tileSheetTex, 16, map1);
	// RenderText("woohoo", 100, 100);
	if(layerOrder == 0){
		DrawMap(furnitureTex, 8, furnitureMap);
		DrawMap(furnitureTex, 8, passableMap);
		// DrawMap(furnitureTex, passableMap);
		DrawCharacter(characterFacing);
	}else if(layerOrder == 1){
		DrawMap(furnitureTex, 8, passableMap);
		DrawCharacter(characterFacing);
		DrawMap(furnitureTex, 8, furnitureMap);
	}else if(layerOrder == 2){
		DrawMap(furnitureTex, 8, furnitureMap);
		DrawCharacter(characterFacing);
		DrawMap(furnitureTex, 8, passableMap);
	}else if(layerOrder == 3){		
		DrawCharacter(characterFacing);
		DrawMap(furnitureTex, 8, passableMap);
		DrawMap(furnitureTex, 8, furnitureMap);
		
	}
	// printf("%d", layerOrder);
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