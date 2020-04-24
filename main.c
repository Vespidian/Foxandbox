#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <time.h>

#include <SDL2/SDL.h>
// #include <SDL_ttf.h>
#include <SDL_image.h>
// #include <SDL_net.h>

#include "headers/DataTypes.h"
#include "headers/initialize.h"
#include "headers/data.h"
#include "headers/drawFunctions.h"
#include "headers/tileMap.h"
#include "headers/collision.h"
#include "headers/inventory.h"
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

bool isWalking = false;
int characterFacing = 1;

void DrawAnimation(SDL_Rect dest, WB_Tilesheet tileSheet, int startFrame, int numFrames, int delay){
	
	int frame = 0;
	frame = (SDL_GetTicks() / delay) % numFrames;
	
	RenderTextureFromSheet(gRenderer, tileSheet, startFrame + frame, dest);
}

void DrawCharacter(int direction, int numFrames){
	SDL_Rect charPos = {characterOffset.x, characterOffset.y, tileSize, tileSize};
	
	if(isWalking){
		DrawAnimation(charPos, characterSheet, (direction) * 4, numFrames, 200);
	}else{
		DrawAnimation(charPos, characterSheet, (direction) * 4, 1, 200);
	}
}

int randArray[32][32];

int darknessMod = 0;
bool isDay = true;

int main(int argc, char **argv) {
	init();
	if(init){
		
		// randomArray();
		Vector2 tmpSize = {WIDTH, HEIGHT};	
		
		SDL_GetWindowSize(gWindow, &WIDTH, &HEIGHT);
		midScreen.x = (WIDTH / 2 - tileSize / 2);
		midScreen.y = (HEIGHT / 2 - tileSize / 2);
		characterOffset = midScreen;
		while(!quit){	
		
			if((SDL_GetTicks() / 10) % 20 == 1){
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
			
			RenderScreen();
			
			const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
			if(currentKeyStates[SDL_SCANCODE_RETURN] || currentKeyStates[SDL_SCANCODE_ESCAPE]){
				quit = true;
			}

			// characterFacing = 0;
			isWalking = false;
			
			if(currentKeyStates[SDL_SCANCODE_A]){
				if(!colLeft){
					worldPosition.x -= 4;
				}
				isWalking = true;
				characterFacing = 4;
			}
			if(currentKeyStates[SDL_SCANCODE_W]){
				if(!colUp){
					worldPosition.y -= 4;
				}
				isWalking = true;
				characterFacing = 2;
			}
			if(currentKeyStates[SDL_SCANCODE_D]){
				if(!colRight){
					worldPosition.x += 4;
				}
				isWalking = true;
				characterFacing = 3;
			}
			if(currentKeyStates[SDL_SCANCODE_S]){
				if(!colDown){
					worldPosition.y += 4;
				}
				isWalking = true;
				characterFacing = 1;
			}
			
			// if(currentKeyStates[SDL_SCANCODE_E]){
				// printf("%d, %d\n", worldPosition.x * 4, worldPosition.y * 4);
			// }
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
					if(e.key.keysym.sym == SDLK_y){
						characterOffset.x = midScreen.x;
						characterOffset.y = midScreen.y;
					}
				}
				if(e.type == SDL_KEYUP){
					// if(e.key.keysym.sym == SDLK_p){
						// PerlinInit();
					// }
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
					if(e.key.keysym.sym == SDLK_e){
						showInv = !showInv;
					}
				}
				if(e.type == SDL_MOUSEWHEEL){
					if(e.wheel.y > 0){
						if(selectedHotbar <= INV_WIDTH && selectedHotbar > 1){
							selectedHotbar--;
						}else{
							selectedHotbar = INV_WIDTH;
						}
					}else if(e.wheel.y < 0){
						if(selectedHotbar < INV_WIDTH && selectedHotbar >= 1){
							selectedHotbar++;
						}else{
							selectedHotbar = 1;
						}
					}
				}
				if(e.type == SDL_WINDOWEVENT){
					if(e.window.event == SDL_WINDOWEVENT_RESIZED){
						SDL_GetWindowSize(gWindow, &WIDTH, &HEIGHT);
						Vector2 diff = {WIDTH - tmpSize.x, HEIGHT - tmpSize.y};
						// Vector2 diff = {tmpSize.x - WIDTH, tmpSize.y - HEIGHT};
						worldPosition.x -= diff.x / 4;
						worldPosition.y -= diff.y / 4;
						// characterOffset = (Vector2){WIDTH / 2 - 8, HEIGHT / 2 - 8};
						characterOffset.x = WIDTH / 2 - tileSize / 2;
						characterOffset.y = HEIGHT / 2 - tileSize / 2;
						tmpSize = (Vector2){WIDTH, HEIGHT};
					}
				}
				
				if(e.type == SDL_QUIT){
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
	clearScreen(gRenderer);
	
	SDL_RenderCopy(gRenderer, backgroundTex, NULL, NULL);
	//Call SDL draw functions here and call RenderScreen from the main loop
	DrawMap(defSheet, map);
	DrawMap(defSheet, map1);

	// DrawMap(furnitureSheet, randArray);
	if(layerOrder == 0){
		DrawMap(furnitureSheet, furnitureMap);
		DrawMap(furnitureSheet, passableMap);
		DrawCharacter(characterFacing, 4);
		
	}else if(layerOrder == 1){
		DrawMap(furnitureSheet, passableMap);
		DrawCharacter(characterFacing, 4);
		DrawMap(furnitureSheet, furnitureMap);
		
	}else if(layerOrder == 2){
		DrawMap(furnitureSheet, furnitureMap);
		DrawCharacter(characterFacing, 4);
		DrawMap(furnitureSheet, passableMap);
		
	}else if(layerOrder == 3){		
		DrawCharacter(characterFacing, 4);
		DrawMap(furnitureSheet, passableMap);
		DrawMap(furnitureSheet, furnitureMap);
		
	}else if(layerOrder == 4){		
		DrawCharacter(characterFacing, 4);
		DrawMap(furnitureSheet, furnitureMap);
		DrawMap(furnitureSheet, passableMap);
		
	}
	
	//Render the player's hitbox
	SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, 0);
	if(enableHitboxes){
		SDL_RenderDrawRect(gRenderer, &charCollider_top);
		SDL_RenderDrawRect(gRenderer, &charCollider_bottom);
		SDL_RenderDrawRect(gRenderer, &charCollider_right);
		SDL_RenderDrawRect(gRenderer, &charCollider_left);
	}
	
	FindCollisions();
	
	INV_DrawInv();
	
	// char numString[20];
	// itoa(123456789, numString, 10);
	// RenderText(numString, 0, 0);
	
	// char posx[20]; 
	// char posy[20]; 
	// char spacer[4] = " : ";
	// itoa(worldPosition.x / 4, posx, 10);
	// itoa(worldPosition.y / 4, posy, 10);
	
	// strcat(posx, spacer);
	// strcat(posx, posy);
	
	char coordinates[256];
	char charoff[256];
	snprintf(coordinates, 1024, "x: %d, y: %d", worldPosition.x / 4, worldPosition.y / 4);
	snprintf(charoff, 1024, "x: %d, y: %d", characterOffset.x, characterOffset.y);
	
	RenderText(gRenderer, coordinates, 0, 0);
	RenderText(gRenderer, charoff, 0, 32);
	// RenderText(gRenderer, "THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG. S \nthe quick brown fox jumped over the lazy dog. s \n1234567890-= \n!@#$%^&*()_+ \n[]{};':,./<>?\\~`", 0, 0);
	// RenderText(gRenderer, "void RenderText(SDL_Renderer *renderer, char *text, int x, int y){\n	testing 1	2;\n}\nint	 a = 50;\nbool isAlive = true;", 0, 400);
	// RenderText(gRenderer, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 0, 200);
	// RenderText(gRenderer, "Hello, World!", 0, 300);
	SDL_RenderCopy(gRenderer, colorModTex, NULL, NULL);
	
	SDL_RenderPresent(gRenderer);
}

int tracking = 9;//Spacing between letters
int spacing = 16;//Vertical spacing
int RenderText(SDL_Renderer *renderer, char *text, int x, int y){
	SDL_Rect charRect = {x, y, 16, 16};
	if(strlen(text) > 0){
		for(int i = 0; i < strlen(text); i++){
			int charVal = (int)text[i] - (int)' ';
			if(charVal >= 0){				
				RenderTextureFromSheet(renderer, fontSheet, charVal, charRect);
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
	return 0;
}

void randomArray(){
	for(int y = 0; y < 32; y++){
		for(int x = 0; x < 32; x++){
			if(getRnd(0, 10) == 1){
				randArray[y][x] = getRnd(10, 13);
			}else{
				randArray[y][x] = -1;
			}
		}
	}
}