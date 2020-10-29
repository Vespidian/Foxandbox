#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <luaconf.h>

#include "headers/DataTypes.h"
#include "headers/ECS.h"
#include "headers/initialize.h"
#include "headers/lua_systems.h"
#include "headers/render_systems.h"
#include "headers/data.h"
#include "headers/level_systems.h"
#include "headers/entity_systems.h"
#include "headers/inventory.h"
#include "headers/map_generation.h"
// Normally SDL2 will redefine the main entry point of the program for Windows applications
// this doesn't seem to play nice with TCC, so we just undefine the redefinition
#ifdef __TINYC__
    #undef main
#endif

//FUNCTION PREDEFINITIONS
void RenderScreen();
void RenderStartScreen();

//Debug Variables
bool quickExit = false;

//MISC
bool quit = false;
static const int targetFramerate = 60;
float deltaTime = 0;
float loopStartTime = 0;
int tileSize = 64;

Vector2 mapOffsetPos = {0, 0};//Offset of the map to simulate movement
Vector2 playerCoord = {0, 0};//Player's coordinate on map
Vector2 placeLocation = {0, 0};
Vector2 characterOffset = {0, 0};//Position of character sprite relative to window 0,0
Vector2 midScreen = {0, 0};
TransformComponent mouseTransform;

bool enableHitboxes = false;

int inputMode = 0;
int numInputModes = 2;
bool isWalking = false;
int characterFacing = 0;
bool uiInteractMode = false;
bool reachLimit = true;
int reachDistance = 7;

SDL_Rect mapRect = {0, 0, 0, 0};

float playerSpeed = 2;

bool mouseClicked = false;
bool mouseHeld = false;
bool showDebugInfo = true;
bool levelLoaded = false;
bool gamePaused = false;

ParticleSystem pSys1;
int chatLogSize = 0;
char **chatHistory;
const int particleCap = 10000;
const int tileStretchSize = 64;
unsigned int submenuIndex = 0;

void clearScreen(SDL_Renderer *renderer){
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
	SDL_RenderClear(renderer);
}

void DrawAnimation(SDL_Rect dest, TilesheetComponent *tileSheet, int startFrame, int numFrames, int delay){
	int frame = 0;
	frame = (SDL_GetTicks() / delay) % numFrames;
	
	AddToRenderQueue(renderer, tileSheet, startFrame + frame, dest, -1, RNDRLYR_PLAYER);
}

void DrawCharacter(int direction, int numFrames){
	SDL_Rect charPos = {characterOffset.x, characterOffset.y, tileSize, tileSize};
	
	if(isWalking){
		DrawAnimation(charPos, find_tilesheet("character"), (direction) * 6, numFrames, 100);
	}else{
		DrawAnimation(charPos, find_tilesheet("character"), (direction + 2) * 6, numFrames, 100);
	}
}

void ResizeWindow(Vector2 previousSize){
	Vector2 roundSpeed = {mapOffsetPos.x % 4, mapOffsetPos.y % 4};/*Make sure the character position is always a 
																	multiple of 4, keeping everything pixel perfect*/
	if(roundSpeed.x != 0){
		mapOffsetPos.x -= roundSpeed.x;
	}
	if(roundSpeed.y != 0){
		mapOffsetPos.y -= roundSpeed.y;
	}

	clearScreen(renderer);
	SDL_GetWindowSize(window, &WIDTH, &HEIGHT);
	Vector2 diff = {WIDTH - previousSize.x, HEIGHT - previousSize.y};
	
	mapOffsetPos.x -= diff.x / 2;
	mapOffsetPos.y -= diff.y / 2;

	midScreen = (Vector2){(WIDTH / 2 - tileSize / 2), (HEIGHT / 2 - tileSize / 2)};
	characterOffset.x += diff.x / 2;
	characterOffset.y += diff.y / 2;
}

void FullscreenWindow(){
	Vector2 size = {WIDTH, HEIGHT};
	SDL_DisplayMode gMode;
	SDL_GetDesktopDisplayMode(0, &gMode);
	WIDTH = gMode.w;
	HEIGHT = gMode.h;
	SDL_SetWindowBordered(window, false);
	SDL_SetWindowPosition(window, 0, 0);
	SDL_SetWindowSize(window, WIDTH, HEIGHT);
	ResizeWindow(size);
}

char currentCollectedText[128] = "";
char consoleOutput[512] = "";
void ParseConsoleCommand(char *command){
	if(command[0] == '/'){
		// strcpy(command, strshft_l(command, 1));
		strshft_l(command, 1);

		if(strcmp(command, "help") == 0){
			strcpy(consoleOutput, "Possible commands:\ndebug lightcycle hitbox noclip\nreachlimit fullscreen");
		}
		if(strcmp(command, "debug") == 0){
			showDebugInfo = !showDebugInfo;
		}
		if(strcmp(command, "hitbox") == 0){
			enableHitboxes = !enableHitboxes;
		}
		if(strcmp(command, "noclip") == 0){
			character.collider.noClip = !character.collider.noClip;
		}
		if(strcmp(command, "reachlimit") == 0){
			reachLimit = !reachLimit;
		}
		if(strcmp(command, "fullscreen") == 0){
			FullscreenWindow();
		}
	}
}

Vector2 modVector2(Vector2 pos, int multiple){
	Vector2 diff = {pos.x % multiple, pos.y % multiple};

	if(diff.x != 0){
		pos.x -= diff.x;
	}
	if(diff.y != 0){
		pos.y -= diff.y;
	}
	return pos;
}

void Setup(){
	SDL_SetRenderDrawColor(renderer, 47, 140, 153, 255);
	SDL_RenderClear(renderer);
	int splashTextSize = 384;
	SDL_Rect splashDest = {WIDTH / 2 - splashTextSize / 2, HEIGHT / 2 - splashTextSize / 2, splashTextSize, splashTextSize};
	SDL_RenderCopy(renderer, loadScreenTex, NULL, &splashDest);
	SDL_RenderPresent(renderer);

	levels = calloc(1, sizeof(LevelComponent));//TEMP

	loadLua();
	droppedItems = malloc(sizeof(DroppedItemComponent) + 1);


	// LoadLevel("data/maps/testmap.dat");
	activeLevel = &levels[numLevels];
	InitializeBlankLevel(activeLevel, (Vector2){32, 32});
	GenerateProceduralMap(activeLevel, 50, 10);
	// UnloadLevel(activeLevel);
	// SaveLevel(activeLevel, "data/maps/testMap.dat");

	SDL_GetWindowSize(window, &WIDTH, &HEIGHT);
	midScreen = (Vector2){(WIDTH / 2 - tileSize / 2), (HEIGHT / 2 - tileSize / 2)};
	characterOffset = midScreen;

	// NewParticleSystem(&pSys1, 1, (SDL_Rect){0, 0, WIDTH, HEIGHT}, 1000, (Range)/*x*/{-1, 1}, (Range)/*y*/{1, 1}, (Range){20, 70});//Snow
	// NewParticleSystem(&pSys1, 2, (SDL_Rect){0, 0, WIDTH, HEIGHT}, 1000, (Range)/*x*/{0, 0}, (Range)/*y*/{5, 6}, (Range){20, 70});//Rain
	// pSys1.boundaryCheck = true;
	// pSys1.fade = false;
	// pSys1.playSystem = false;
}

void e_CheckMouseLayer(SDL_Event e){
	//Set the mouseEditing pointer to the layer the mouse is currently holding on
	if(!showInv){
		if(e.button.button == SDL_BUTTON_RIGHT){
			if(activeLevel->features[mouseTransform.tilePos.y][mouseTransform.tilePos.x].block != find_block("air")){
				mouseEditingLayer = activeLevel->features;
			}else{
				mouseEditingLayer = activeLevel->terrain;
			}
		}else if(e.button.button == SDL_BUTTON_LEFT){
			if(strcmp(find_block(invArray[selectedHotbar].item->name)->layer, "terrain") == 0){
				mouseEditingLayer = activeLevel->terrain;
			}else{
				mouseEditingLayer = activeLevel->features;
			}
		}
	}
}

int main(int argc, char **argv) {
	if(init()){
		Setup();
		while(!quit){
			loopStartTime = SDL_GetTicks();
			const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
			
			mouseClicked = false;
			mouseHeld = false;
			if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT) || SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)){
				mouseHeld = true;
			}
			if(currentKeyStates[SDL_SCANCODE_ESCAPE] && quickExit){
				quit = true;
			}
			while(SDL_PollEvent(&e) != 0){
				SDL_GetMouseState(&mouseTransform.screenPos.x, &mouseTransform.screenPos.y);
				switch(e.type){
					case SDL_MOUSEBUTTONDOWN:
						if(e.key.state == SDL_RELEASED){
							mouseClicked = true;
						}

						break;

					case SDL_WINDOWEVENT:
						if(e.window.event == SDL_WINDOWEVENT_RESIZED){
							ResizeWindow((Vector2){WIDTH, HEIGHT});
						}
						break;

					case SDL_KEYUP:
						if(inputMode == 0){
							if(e.key.keysym.sym == SDLK_b){//Fullscreen
								FullscreenWindow();
							}
						}

						break;

					case SDL_QUIT:
						quit = true;

						break;
				}

				if(levelLoaded){
					if(e.type == SDL_KEYDOWN){
						if(e.key.keysym.sym == SDLK_ESCAPE){
							gamePaused = !gamePaused;
						}
					}
					if(!gamePaused){
						switch(e.type){
							case SDL_MOUSEBUTTONDOWN:
								e_CheckMouseLayer(e);

								break;

							case SDL_KEYDOWN:
								if(inputMode == 0){//Normal input
									//Hotbar slot selection via number keys
									if(e.key.keysym.sym >= 49 && e.key.keysym.sym <= 57 && e.key.keysym.sym - 49 < INV_WIDTH){
										selectedHotbar = e.key.keysym.sym - 49;
									}
								}else if(inputMode == 1){//Text Input
									if(e.key.keysym.sym == SDLK_BACKSPACE){
										currentCollectedText[strlen(currentCollectedText) - 1] = '\0';
									}
								}

								break;

							case SDL_KEYUP:
								mapOffsetPos = modVector2(mapOffsetPos, 4);
								characterOffset = modVector2(characterOffset, 4);
								
								if(e.key.keysym.sym == SDLK_RETURN){//Chat history
									if(inputMode == 1){
										if(currentCollectedText[0] != '\0' && currentCollectedText[0] != ' '){
											chatHistory = realloc(chatHistory, (chatLogSize + 1) * sizeof(char **));
											chatHistory[chatLogSize] = calloc(strlen(currentCollectedText) + 1, sizeof(char));
											strcpy(chatHistory[chatLogSize], currentCollectedText);
											chatLogSize++;
											currentCollectedText[0] = '\0';
											printf("%s\n", chatHistory[chatLogSize - 1]);
											ParseConsoleCommand(chatHistory[chatLogSize - 1]);
										}
									}
									if(inputMode < numInputModes - 1){
										inputMode++;
										
									}else{
										inputMode = 0;
									}
								}
								if(inputMode == 0){
									if(e.key.keysym.sym == SDLK_F3){
										showDebugInfo = !showDebugInfo;
									}
									if(e.key.keysym.sym == SDLK_F10){//Memory leak possible
										loadLua();
									}
									if(e.key.keysym.sym == SDLK_j){
										SaveLevel(activeLevel, "data/maps/testMap.dat");
									}
									if(e.key.keysym.sym == SDLK_c){
										character.collider.noClip = !character.collider.noClip;
									}
									if(e.key.keysym.sym == SDLK_x){
										enableHitboxes = !enableHitboxes;
									}
									if(e.key.keysym.sym == SDLK_e){
										showInv = !showInv;
									}
									if(e.key.keysym.sym == SDLK_h){
										mapOffsetPos.x += characterOffset.x - midScreen.x;
										mapOffsetPos.y += characterOffset.y - midScreen.y;
										characterOffset = midScreen;
									}
								}

								break;

							case SDL_MOUSEWHEEL:
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

								break;
						}

						if(inputMode == 1){
							if(e.type == SDL_TEXTINPUT){
								strcat(currentCollectedText, e.text.text);
							}
						}
					}
				}
			}


			if(levelLoaded){
				if(!gamePaused){
					if(characterFacing == 4){
						placeLocation = (Vector2){-1, 0};
					}else if(characterFacing == 3){
						placeLocation = (Vector2){1, 0};
					}else if(characterFacing == 2){
						placeLocation = (Vector2){0, -1};
					}else if(characterFacing == 1){
						placeLocation = (Vector2){0, 1};
					}
					
					int freeRoamDistance = 32;
					if(currentKeyStates[SDL_SCANCODE_Q]){
						SDL_Rect freeRoamRect = {midScreen.x - freeRoamDistance, midScreen.y - freeRoamDistance, freeRoamDistance * 2 + 64, freeRoamDistance * 2 + 64};
						AddToRenderQueue(renderer, &debugSheet, 6, freeRoamRect, 255, RNDRLYR_UI);
					}
					isWalking = false;
					float playerMovementSpeed = playerSpeed * deltaTime;

					if(inputMode == 0){
						if(!(currentKeyStates[SDL_SCANCODE_A] && currentKeyStates[SDL_SCANCODE_D])){
							if(currentKeyStates[SDL_SCANCODE_A] && !character.collider.colLeft){
								if(midScreen.x - characterOffset.x < freeRoamDistance){
									characterOffset.x -= playerMovementSpeed;
								}else{
									mapOffsetPos.x -= playerMovementSpeed;
								}

								isWalking = true;
								characterFacing = 0;
							}
							if(currentKeyStates[SDL_SCANCODE_D] && !character.collider.colRight){
								if(characterOffset.x - midScreen.x < freeRoamDistance){
									characterOffset.x += playerMovementSpeed;
								}else{
									mapOffsetPos.x += playerMovementSpeed;
								}
								isWalking = true;
								characterFacing = 1;
							}
						}
						if(!(currentKeyStates[SDL_SCANCODE_W] && currentKeyStates[SDL_SCANCODE_S])){
							if(currentKeyStates[SDL_SCANCODE_W] && !character.collider.colUp){
								if(midScreen.y - characterOffset.y < freeRoamDistance){
									characterOffset.y -= playerMovementSpeed;
								}else{
									mapOffsetPos.y -= playerMovementSpeed;
								}
								isWalking = true;
							}
							if(currentKeyStates[SDL_SCANCODE_S] && !character.collider.colDown){
								if(characterOffset.y - midScreen.y < freeRoamDistance){
									characterOffset.y += playerMovementSpeed;
								}else{
									mapOffsetPos.y += playerMovementSpeed;
								}
								isWalking = true;
							}
						}
					}

					mapRect = (SDL_Rect){-mapOffsetPos.x, -mapOffsetPos.y, activeLevel->map_size.x * 64, activeLevel->map_size.y * 64};	
				}
				RenderScreen();
			}else{
				RenderStartScreen();
			}

			SDL_Delay(1000 / targetFramerate);//Game FrameRate
			deltaTime = (SDL_GetTicks() - loopStartTime) / 10;
		}
	}else{
		printf("Failed to initialize\n");
	}
	
	if(quit){
		Quit();
	}
	return 0;
}

typedef void (*func)(int a);

typedef struct{
	char *text;
	func a;
}Button;

typedef struct{
	int numElements;
	int spacing;
	Vector2 origin;
	Vector2 size;
	int lastInteracted;
}ButtonMenu;

bool b1 = false;
bool b2 = false;
bool b3 = false;


int optionsSubmenu = 0;
void RenderStartScreen(){
	SDL_SetRenderDrawColor(renderer, 139, 214, 239, 255);
	SDL_RenderClear(renderer);

	Vector2 buttonSize = {128, 32};
	Vector2 origin = {84, HEIGHT / 2};

	Vector2 checkboxSize = {160, 32};
	// Vector2 cOrigin = {300, HEIGHT / 2};
	// SDL_Rect orig = {origin.x - 4, origin.y - 4, 8, 8};
	int spacing = 6;

	switch(submenuIndex){
		case 1:
			if(DrawButton(renderer, "General", VerticalRectList(5, 0, buttonSize, origin, spacing))){optionsSubmenu = 0;}
			if(DrawButton(renderer, "Video", VerticalRectList(5, 1, buttonSize, origin, spacing))){optionsSubmenu = 1;}
			if(DrawButton(renderer, "Audio", VerticalRectList(5, 2, buttonSize, origin, spacing))){optionsSubmenu = 2;}
			if(DrawButton(renderer, "Controls", VerticalRectList(5, 3, buttonSize, origin, spacing))){optionsSubmenu = 3;}
			if(DrawButton(renderer, "Back", VerticalRectList(5, 4, buttonSize, origin, spacing))){submenuIndex = 0;}

			origin = (Vector2){232, HEIGHT / 2};
			switch(optionsSubmenu){
				case 0:
					if(DrawButton(renderer, "b1", VerticalRectList(6, 0, buttonSize, origin, spacing))){}
					DrawCheckbox(renderer, &b1, "Checkbox 1:", VerticalRectList(6, 1, checkboxSize, origin, spacing));
					if(DrawButton(renderer, "b2", VerticalRectList(6, 2, buttonSize, origin, spacing))){}
					DrawCheckbox(renderer, &b2, "Checkbox 2:", VerticalRectList(6, 3, checkboxSize, origin, spacing));
					DrawCheckbox(renderer, &b3, "Checkbox 3:", VerticalRectList(6, 4, checkboxSize, origin, spacing));
					if(DrawButton(renderer, "b3", VerticalRectList(6, 5, buttonSize, origin, spacing))){}

					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
			}
			break;
		case 2://Options menu
			if(DrawButton(renderer, "Wow cool!", VerticalRectList(2, 0, buttonSize, origin, spacing))){}
			if(DrawButton(renderer, "Back", VerticalRectList(2, 1, buttonSize, origin, spacing))){
				submenuIndex = 0;
			}
			break;
		case 3:
			if(DrawButton(renderer, "Mod Folder", VerticalRectList(2, 0, buttonSize, origin, spacing))){
				#ifdef _WIN32
					system("explorer scripts");//Open the scripts folder in windows explorer
				#endif
			}
			if(DrawButton(renderer, "Back", VerticalRectList(2, 1, buttonSize, origin, spacing))){
				submenuIndex = 0;
			}
			break;
		default://Default start menu
			if(DrawButton(renderer, "Start", VerticalRectList(5, 0, buttonSize, origin, spacing))){
				levelLoaded = true;
			}
			if(DrawButton(renderer, "Options", VerticalRectList(5, 1, buttonSize, origin, spacing))){
				submenuIndex = 1;
			}
			if(DrawButton(renderer, "Temp1", VerticalRectList(5, 2, buttonSize, origin, spacing))){submenuIndex = 2;}
			if(DrawButton(renderer, "Mods", VerticalRectList(5, 3, buttonSize, origin, spacing))){submenuIndex = 3;}
			if(DrawButton(renderer, "Exit", VerticalRectList(5, 4, buttonSize, origin, spacing))){
				quit = true;
			}
			break;
	}


	RenderUpdate();
	SDL_RenderPresent(renderer);
}

void RenderScreen(){
	clearScreen(renderer);
	//Draw background sky
	int texSize = 512;//Size of one background texture-tile
	int tilesX = (WIDTH / texSize) + 1;//Number of tiles that fit on the x axis
	int tilesY = (HEIGHT / texSize) + 1;//Number of tile that fit on the y axis
	SDL_Rect destRect = {0, 0, texSize, texSize};//Template rect to paste each of the textures to screen
	for(int y = 0; y < tilesY + 2; y++){//Nested loop to iterate through tiles (the plus one is there as to make sure to not leave gaps)
		destRect.y = (y - 1) * texSize + -mapOffsetPos.y / 2 % texSize;//set y offset
		for(int x = 0; x < tilesX + 2; x++){
			destRect.x = (x - 1) * texSize + -mapOffsetPos.x / 2 % texSize;//Set x offset
			SDL_RenderCopy(renderer, backgroundTex, NULL, &destRect);//Draw the texture
		}
	}
	//Call SDL draw functions here and call RenderScreen from the main loop
	if(gamePaused){
		RenderPauseMenu();
	}
	DrawLevel();
	RenderDroppedItems();
	INV_DrawInv();
	RenderConsole();
	RenderParticleSystem(pSys1);
	RenderCursor();
	
	FindCollisions();
	
	character.transform.tilePos = (Vector2){
		((characterOffset.x + 32) + mapOffsetPos.x) / 64,
		((characterOffset.y + 32) + mapOffsetPos.y) / 64,
	};
	
	if(showDebugInfo){
		char coordinates[256];
		char charoff[256];
		snprintf(coordinates, 1024, "Player Coordinates ->\nx: %d, y: %d", character.transform.tilePos.x, character.transform.tilePos.y);
		snprintf(charoff, 1024, "MapOffset ->\nx: %d, y: %d", mapOffsetPos.x, mapOffsetPos.y);
		RenderText_d(renderer, coordinates, 0, 0);
		RenderText_d(renderer, charoff, 0, 48);	
		char frameCount[64];
		snprintf(frameCount, 128, "%d RenderCopy calls", renderItemIndex);
		RenderText_d(renderer, frameCount, WIDTH - 200, 0);

		//Performance bar
		AddToRenderQueue(renderer, &debugSheet, 4, (SDL_Rect){WIDTH - 15, 10, 15, deltaTime * 20}, 255, 1000);
	}

	RenderUpdate();
	SDL_RenderPresent(renderer);
	particleCount = 0;
}

void RenderPauseMenu(){
	Vector2 origin = {WIDTH / 2, HEIGHT / 2};
	Vector2 buttonSize = {192, 32};
	int spacing = 8;
	if(DrawButton(renderer, "Resume", VerticalRectList(3, 0, buttonSize, origin, spacing))){gamePaused = false;}
	if(DrawButton(renderer, "To Menu", VerticalRectList(3, 1, buttonSize, origin, spacing))){levelLoaded = false;}
	if(DrawButton(renderer, "Exit To Desktop", VerticalRectList(3, 2, buttonSize, origin, spacing))){quit = true;}
	DrawVRectListBackround(3, buttonSize, origin, spacing, 180);
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