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
#include "headers/ECS.h"
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
int characterFacing = 1;

void DrawAnimation(SDL_Rect dest, WB_Tilesheet tileSheet, int startFrame, int numFrames, int delay){
	
	int frame = 0;
	frame = (SDL_GetTicks() / delay) % numFrames;
	
	AddToRenderQueue(gRenderer, tileSheet, startFrame + frame, dest, RNDRLYR_PLAYER);
}

void DrawCharacter(int direction, int numFrames){
	SDL_Rect charPos = {characterOffset.x, characterOffset.y, tileSize, tileSize};
	
	if(isWalking){
		DrawAnimation(charPos, characterSheet, (direction) * 4, numFrames, 200);
	}else{
		DrawAnimation(charPos, characterSheet, (direction) * 4, 1, 200);
	}
}

SDL_Rect windowRect;


int darknessMod = 0;
bool isDay = true;
bool doDayCycle = false;
Vector2 worldPos = {0, 0};
int playerSpeed = 4;
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



RenderTileComponent tempArray[32][32] = {};
void DefineBorder();

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
				}
				isWalking = true;
				characterFacing = 4;
			}
			if(currentKeyStates[SDL_SCANCODE_W]){
				if(!colUp){
					// if(SDL_GetTicks() % 2 == 0)
						mapOffsetPos.y -= playerSpeed;
				}
				isWalking = true;
				characterFacing = 2;
				// worldPos.y -= 1;
			}
			if(currentKeyStates[SDL_SCANCODE_D]){
				if(!colRight){
					// if(SDL_GetTicks() % 2 == 0)
						mapOffsetPos.x += playerSpeed;
				}
				isWalking = true;
				characterFacing = 3;
				// worldPos.x += 1;
			}
			if(currentKeyStates[SDL_SCANCODE_S]){
				if(!colDown){
					// if(SDL_GetTicks() % 2 == 0)
						mapOffsetPos.y += playerSpeed;
				}
				isWalking = true;
				characterFacing = 1;
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
					
					if(e.key.keysym.sym == SDLK_h){
						noClip = !noClip;
						DefineBorder();

					}
					if(e.key.keysym.sym == SDLK_y){
						SmoothMap();
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
						DefineBorder();
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
	snprintf(coordinates, 1024, "x: %d, y: %d", worldPos.x, worldPos.y);
	snprintf(charoff, 1024, "x: %d, y: %d", characterOffset.x, characterOffset.y);
	
	RenderText_d(gRenderer, coordinates, 0, 0);
	RenderText_d(gRenderer, charoff, 0, 32);	
	
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


void RenderCursor(){
	// Highlight the tile the mouse is currently on
	SDL_GetMouseState(&mousePos.x, &mousePos.y);
	SDL_Point mousePoint = {mousePos.x, mousePos.y};
	// if(SDL_PointInRect(&mousePoint, &tile)){
		Vector2 mapMousePos = {((mousePos.x)) / 64, ((mousePos.y)) / 64};
		SDL_Rect mouseHighlight = {mapMousePos.x * 64 + mapOffsetPos.x % 64, mapMousePos.y * 64 + mapOffsetPos.y % 64, 64, 64};
		// SDL_SetRenderDrawColor(gRenderer, 255, 211, 0, 0xff);
		// SDL_RenderDrawRect(gRenderer, &mouseHighlight);
		AddToRenderQueue(gRenderer, debugSheet, 0, mouseHighlight, RNDRLYR_UI - 1);
		
	// }
}




enum types {GRASS = 0, WATER = 47};
int GetSurroundCount(Vector2 tile){
	int surroundCount = 0;
	for(int y = tile.y - 1; y <= tile.y + 1; y++){
		for(int x = tile.x - 1; x <= tile.x + 1; x++){
			if(x >= 0 && x < 32 && y >= 0 && y < 32){
				if(x != tile.x || y != tile.y){
					if(tempArray[y][x].type == GRASS){
						surroundCount++;
					}
				}
			}
		}
	}
	// printf("%d\n", surroundCount);
	return surroundCount;
}


// void ResetEdges(){
	// for(int y = 0; y < 32; y++){
		// for(int x = 0; x < 32; x++){
			// if(randArray[y][x].type > GRASS && randArray[y][x].type < WATER){
				// randArray[y][x].type = GRASS;
				// printf("wow\n");
			// }
		// }
	// }
// }

void DefineBorder(){
/*
16  |  1  | 32
____|_____|____
 8  | /// |  2
____|_____|____
128 |  4  | 64

*/
	int correlationArray[] = {255, 2, 8, 10, 11, 16, 18, 22, 24, 26, 27, 30, 31, 64, 66, 72, 74, 75, 80, 82, 86, 88, 90, 91, 94, 95, 104, 106, 107, 120, 122, 123, 126, 127, 208, 210, 214, 216, 218, 219, 222, 223, 248, 250, 251, 254, 0};
	for(int y = 0; y < 32; y++){
		for(int x = 0; x < 32; x++){
			int adjacentCaseNumber = 0;			
			int cornerCaseNumber = 0;	
			int finalCaseNumber = 0;
			// int numSurrounds = 0;
			
			
			// Marching Squares:
			/*caseNumber += (tempArray[y + 1][x].type == WATER) << 0;
			caseNumber += (tempArray[y + 1][x + 1].type == WATER) << 1;
			caseNumber += (tempArray[y][x + 1].type == WATER) << 2;
			caseNumber += (tempArray[y][x].type == WATER) << 3;
			randArray[y][x].type = caseNumber;*/
			
			
			//Auto Tiling
			bool topLeft = (tempArray[y + 1][x - 1].type == GRASS);
			bool topRight = (tempArray[y + 1][x + 1].type == GRASS);
			bool bottomLeft = (tempArray[y - 1][x - 1].type == GRASS);
			bool bottomRight = (tempArray[y - 1][x + 1].type == GRASS);
			bool top = (tempArray[y + 1][x].type == GRASS);
			bool bottom = (tempArray[y - 1][x].type == GRASS);
			bool right = (tempArray[y][x + 1].type == GRASS);
			bool left = (tempArray[y][x - 1].type == GRASS);
			if(tempArray[y][x].type == GRASS){
				//Reduce possible options down to 47
				if(topLeft){
					if(!top || !left){
						topLeft = false;
					}
				}
				if(topRight){
					if(!top || !right){
						topRight = false;
					}
				}
				if(bottomLeft){
					if(!bottom || !left){
						bottomLeft = false;
					}
				}
				if(bottomRight){
					if(!bottom || !right){
						bottomRight = false;
					}
				}
				cornerCaseNumber += topLeft << 0;//CRN
				adjacentCaseNumber += top << 1;//ADJ
				cornerCaseNumber += topRight << 2;//CRN
				adjacentCaseNumber += left << 3;//ADJ
				adjacentCaseNumber += right << 4;//ADJ
				cornerCaseNumber += bottomLeft << 5;//CRN
				adjacentCaseNumber += bottom << 6;//ADJ
				cornerCaseNumber += bottomRight << 7;//CRN
				
				finalCaseNumber = adjacentCaseNumber + cornerCaseNumber;
				
				//Order the 47 possible options in a compact way
				for(int i = 0; i < 47; i++){
					if(finalCaseNumber == correlationArray[i]){
						finalCaseNumber = i;
						break;
					}
				}
				
				// printf("%d\n", finalCaseNumber);
				randArray[y][x].type = finalCaseNumber;
				
			}
			// if(finalCaseNumber >= GRASS && finalCaseNumber < WATER){
			if(randArray[y][x].type == WATER){
				// colMap[y][x] = -1;
			// }else{
				colMap[y][x] = 0;
			}
		}
		// printf("\n");
	}
}

void SmoothMap(){
	for(int y = 0; y < 32; y++){
		for(int x = 0; x < 32; x++){
			tempArray[y][x] = randArray[y][x];
		}
	}
	for(int y = 0; y < 32; y++){
		for(int x = 0; x < 32; x++){
			int surroundTiles = GetSurroundCount((Vector2){x, y});
			
			if(surroundTiles > 4){
				randArray[y][x].type = GRASS;
				// colMap[y][x] = -1;
			}else if(surroundTiles < 4){
				randArray[y][x].type = WATER;
				// colMap[y][x] = 0;
			}
		}
	}
}
void randomArray(){
	int percentGrass = 50;
	time_t rawTime;
	struct tm *timeinfo;
	time(&rawTime);
	timeinfo = localtime(&rawTime);
	int worldSeed = timeinfo->tm_sec;
	SeedLehmer(worldSeed, 0, 0);
	for(int y = 0; y < 32; y++){
		for(int x = 0; x < 32; x++){
			if(getRnd(0, 100) <= percentGrass){
				randArray[y][x].type = GRASS;
			}else{
				randArray[y][x].type = WATER;
			}
		}
	}
	// SmoothMap();
}










Vector2 ConvertToWorldPos(Vector2 pos){
	return (Vector2){pos.x - mapOffsetPos.x, pos.y - mapOffsetPos.y};
}

void RenderTextureInWorld(SDL_Renderer *renderer, WB_Tilesheet sheet, int tile, SDL_Rect rect, int zPos){
	rect.x -= mapOffsetPos.x;
	rect.y -= mapOffsetPos.y;
	AddToRenderQueue(renderer, sheet, tile, rect, zPos);
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
