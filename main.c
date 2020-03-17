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

SDL_Rect charCollider;

int layerOrder = 0;
// bool isBehind = false;

char *characterFacing = "down";

void DrawCharacter(char *direction){
	SDL_Rect charPos = {(WIDTH / 2 - tileSize / 2), (HEIGHT / 2 - tileSize / 2), tileSize, tileSize};
	
	if(strcmp(direction, "up") == 0){
		RenderTextureFromSheet(gRenderer, characterTex, 4, 16, 0, charPos);
		
	}else if(strcmp(direction, "down") == 0){
		RenderTextureFromSheet(gRenderer, characterTex, 4, 16, 1, charPos);
		
	}else if(strcmp(direction, "left") == 0){
		RenderTextureFromSheet(gRenderer, characterTex, 4, 16, 2, charPos);
		
	}else if(strcmp(direction, "right") == 0){
		RenderTextureFromSheet(gRenderer, characterTex, 4, 16, 3, charPos);
		
	}
}


int main(int argc, char **argv) {
	init();
	
	if(init){
		while(!quit){			
			RenderScreen();
			
			bool colUp = false;
			bool colDown = false;
			bool colLeft = false;
			bool colRight = false;
			
			SDL_Rect charCollider = {(WIDTH / 2 - tileSize / 2), (HEIGHT / 2 - tileSize / 2), tileSize, tileSize};
			
			/* DEFAULT SQUARE PLAYER COLLIDER
			SDL_Rect charCollider_bottom = {(WIDTH / 2 - tileSize / 2), (HEIGHT / 2 - tileSize / 2) + tileSize, tileSize, 1};
			SDL_Rect charCollider_right = {(WIDTH / 2 - tileSize / 2) + tileSize, (HEIGHT / 2 - tileSize / 2), 1, tileSize};
			SDL_Rect charCollider_left = {(WIDTH / 2 - tileSize / 2) - 1, (HEIGHT / 2 - tileSize / 2), 1, tileSize};
			SDL_Rect charCollider_top = {(WIDTH / 2 - tileSize / 2), (HEIGHT / 2 - tileSize / 2) - 1, tileSize, 1};
			*/
			
			
			SDL_Rect charCollider_bottom = {(WIDTH / 2 - tileSize / 2) + 4, (HEIGHT / 2 - tileSize / 2) + tileSize, tileSize - 8, 1};
			SDL_Rect charCollider_right = {(WIDTH / 2 - tileSize / 2) + tileSize - 4, (HEIGHT / 2 - tileSize / 2) + 60, 1, 4};
			SDL_Rect charCollider_left = {(WIDTH / 2 - tileSize / 2) + 3, (HEIGHT / 2 - tileSize / 2) + 60, 1, 4};
			SDL_Rect charCollider_top = {(WIDTH / 2 - tileSize / 2) + 4, (HEIGHT / 2 - tileSize / 2) + 59, tileSize - 8, 1};
			
			//Render the player's hitbox
			SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, 0);
			// SDL_RenderDrawRect(gRenderer, &charCollider_top);
			// SDL_RenderDrawRect(gRenderer, &charCollider_bottom);
			// SDL_RenderDrawRect(gRenderer, &charCollider_right);
			// SDL_RenderDrawRect(gRenderer, &charCollider_left);
			
			for(int y = 0; y < 32; y++){
				for(int x = 0; x < 32; x++){
					SDL_Rect tileR = {(x * tileSize) + worldPosition.x, (y * tileSize) + worldPosition.y, tileSize, tileSize};
					if(colMap[y][x] == 0){
						SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, 0);
						if(SDL_HasIntersection(&tileR, &charCollider_top)){
							colUp = true;
						}
						if(SDL_HasIntersection(&tileR, &charCollider_bottom)){
							colDown = true;
						}
						if(SDL_HasIntersection(&tileR, &charCollider_left)){
							colLeft = true;
						}
						if(SDL_HasIntersection(&tileR, &charCollider_right)){
							colRight = true;
						}
					}else if(colMap[y][x] == 1){
						SDL_SetRenderDrawColor(gRenderer, 0, 0, 150, 0);
						//Only put the character behind if the character is intersecting with said tile
						if(SDL_HasIntersection(&tileR, &charCollider)){
							if(tileR.y + tileSize < charCollider_bottom.y){
								// isBehind = true;
								layerOrder = 0;
							}else{
								layerOrder = 1;
								// isBehind = false;
							}
						}
					}else if(colMap[y][x] == 4){
						SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, 0);
						if(SDL_HasIntersection(&tileR, &charCollider)){
							if(tileR.y + tileSize < charCollider_bottom.y){
								// isBehind = true;
								layerOrder = 0;
							}else{
								// isBehind = false;
								layerOrder = 2;
							}
						}
						
						tileR.y = (y * tileSize) + worldPosition.y + tileSize / 2;
						tileR.h = tileSize / 2;
						if(SDL_HasIntersection(&tileR, &charCollider_top)){
							colUp = true;
						}
						if(SDL_HasIntersection(&tileR, &charCollider_bottom)){
							colDown = true;
						}
						if(SDL_HasIntersection(&tileR, &charCollider_left)){
							colLeft = true;
						}
						if(SDL_HasIntersection(&tileR, &charCollider_right)){
							colRight = true;
						}
					}
					if(colMap[y][x] != -1){
						// SDL_RenderDrawRect(gRenderer, &tileR);
					}
				}
			}
			
			SDL_RenderPresent(gRenderer);
			const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
			if(currentKeyStates[SDL_SCANCODE_RETURN] || currentKeyStates[SDL_SCANCODE_ESCAPE]){
				quit = true;
			}

			if(currentKeyStates[SDL_SCANCODE_W] && !colUp){
				worldPosition.y += 4;
				characterFacing = "up";
			}
			if(currentKeyStates[SDL_SCANCODE_A] && !colLeft){
				worldPosition.x += 4;		
				characterFacing = "left";
			}
			if(currentKeyStates[SDL_SCANCODE_D] && !colRight){
				worldPosition.x -= 4;
				characterFacing = "right";
			}
			if(currentKeyStates[SDL_SCANCODE_S] && !colDown){
				worldPosition.y -= 4;
				characterFacing = "down";
			}
			
			if(currentKeyStates[SDL_SCANCODE_E]){
				map[5][4] = 7;
				printf("%d, %d\n", -worldPosition.x, -worldPosition.y + HEIGHT/2);
			}
			
			
			while(SDL_PollEvent(&e) != 0){				
				if(e.type == SDL_KEYDOWN){
					if(e.key.keysym.sym == SDLK_t){
						TextureInit();
						MapInit();
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
	DrawMap(tileSheetTex, map);
	// RenderText("woohoo", 100, 100);
	if(layerOrder == 0){
		DrawMap(furnitureTex, furnitureMap);
		DrawMap(furnitureTex, passableMap);
		// DrawMap(furnitureTex, passableMap);
		DrawCharacter(characterFacing);
	}else if(layerOrder == 1){
		DrawMap(furnitureTex, furnitureMap);
		DrawCharacter(characterFacing);
		DrawMap(furnitureTex, passableMap);
		// DrawMap(furnitureTex, passableMap);
	}else if(layerOrder == 2){
		// SDL_RenderCopy(gRenderer, characterTex, NULL, &charPos);
		
		DrawCharacter(characterFacing);
		
		DrawMap(furnitureTex, furnitureMap);
		DrawMap(furnitureTex, passableMap);
	}
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