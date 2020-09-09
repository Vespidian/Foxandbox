#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// extern "C"{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <luaconf.h>
// }

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
fVector2 characterOffset = {0, 0};//Position of character sprite relative to window 0,0
// Vector2 mouseTransform.screenPos = {0, 0};//Position of the mouse
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

SDL_Rect windowRect;

int darknessMod = 0;
bool isDay = true;
bool doDayCycle = false;
float playerSpeed = 2;

bool mouseClicked = false;
bool mouseHeld = false;
bool showDebugInfo = true;

ParticleSystem pSys1;
int chatLogSize = 0;
char **chatHistory;

void clearScreen(SDL_Renderer *renderer){
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
	SDL_RenderClear(renderer);

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
}

void DrawAnimation(SDL_Rect dest, WB_Tilesheet tileSheet, int startFrame, int numFrames, int delay){
	int frame = 0;
	frame = (SDL_GetTicks() / delay) % numFrames;
	
	AddToRenderQueue(renderer, tileSheet, startFrame + frame, dest, -1, RNDRLYR_PLAYER);
}

WB_Tilesheet *tilesheets = NULL;
int num_tilesheets = 0;

WB_Tilesheet *find_tilesheet(char *name){
	for(int i = 0; i < num_tilesheets; i++){
		if(strcmp(name, tilesheets[i].name) == 0){
			return &tilesheets[i];
		}
	}
	return &undefinedSheet;
}
void DrawCharacter(int direction, int numFrames){
	SDL_Rect charPos = {characterOffset.x, characterOffset.y, tileSize, tileSize};
	
	if(isWalking){
		DrawAnimation(charPos, *find_tilesheet("character"), (direction) * 6, numFrames, 100);
	}else{
		DrawAnimation(charPos, *find_tilesheet("character"), (direction + 2) * 6, numFrames, 100);
	}
}


int runScript(char *fileName){
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	luaL_dofile(L, fileName);
	lua_close(L);
	return 0;
}

int num_from_table(lua_State *L, char *field){
	luaL_checktype(L, 2, LUA_TTABLE);//Push table into the stack
	lua_getfield(L, -1, field);//Find the field of the table
	int returnValue = -1;
	if(lua_type(L, -1) == LUA_TNUMBER){//Make sure its a number
		returnValue = lua_tonumber(L, -1);
	}
	return returnValue;
}

int register_tilesheet(lua_State *L){
	tilesheets = realloc(tilesheets, (num_tilesheets + 1) * sizeof(WB_Tilesheet));
	
	luaL_checktype(L, 1, LUA_TTABLE);
	lua_getfield(L, -1, "name");
	// if(strcmp(*find_tilesheet(lua_tostring(L, -1))->name, "undefined") == 0){
		if(lua_tostring(L, -1) != NULL){
			strcpy(tilesheets[num_tilesheets].name, lua_tostring(L, -1));
		}else{
			strcpy(tilesheets[num_tilesheets].name, "undefined");
		}
		
		lua_getfield(L, -2, "path");
		if(lua_tostring(L, -1) != NULL){
			tilesheets[num_tilesheets].tex = IMG_LoadTexture(renderer, lua_tostring(L, -1));
		}else{
			tilesheets[num_tilesheets].tex = IMG_LoadTexture(renderer, "undefined");
		}

		lua_getfield(L, -3, "tile_size");
		if(lua_tonumber(L, -1)){
			tilesheets[num_tilesheets].tile_size = lua_tonumber(L, -1);
			//Identify the width and height of the tilesheet based on tile size
			SDL_QueryTexture(tilesheets[num_tilesheets].tex, NULL, NULL, &tilesheets[num_tilesheets].w, &tilesheets[num_tilesheets].h);
			tilesheets[num_tilesheets].w /= tilesheets[num_tilesheets].tile_size;
			tilesheets[num_tilesheets].h /= tilesheets[num_tilesheets].tile_size;
		}

		num_tilesheets++;
	// }
	return 0;
}

int pass_table(lua_State *L){
	char **temp;
	temp = malloc(2 * sizeof(char[8]));
	// char temp[8][32] = {};
	printf("%s flags:\n", lua_tostring(L, 1));
	
	//Get flags
	luaL_checktype(L, 2, LUA_TTABLE);
	size_t ec = 0;
	lua_pushnil(L);
	while(lua_next(L, 2)){
		if(lua_type(L, -1) == LUA_TSTRING){//Only accept strings
			char *tmp = malloc(sizeof(char *));
			strcpy(tmp, lua_tostring(L, -1));
			temp[ec] = malloc(sizeof(char[strlen(tmp)]));
			strcpy(temp[ec], tmp);
			ec++;
			free(tmp);
		}
		lua_pop(L, 1);
	}
	
	
	printf("%d\n", num_from_table(L, "tile"));
	
	//Print flags
	for(int i = 0; i < ec; i++){
		printf("%s\n", temp[i]	);
	}
	
	//Return value
	lua_pushinteger(L, (lua_Integer)ec);
	free(*temp);
	return 1;
}

void loadLua(){
	num_tilesheets = 0;
	numItems = -1;
	numBlocks = -1;
	numAutotiles = -1;

	tilesheets = malloc(sizeof(WB_Tilesheet));
	itemData = malloc(sizeof(ItemComponent));
	blockData = malloc(sizeof(BlockComponent));
	autotileData = malloc(sizeof(AutotileComponent));
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	
	lua_register(L, "register_item", register_item);
	lua_register(L, "register_tilesheet", register_tilesheet);
	lua_register(L, "register_block", register_block);
	lua_register(L, "populate_autotile", populate_autotile);
	
	luaL_dofile(L, "scripts/init.lua");	
	lua_close(L);
}


void NewEntity();
void RenderEntities();
void Setup(){
	SDL_SetRenderDrawColor(renderer, 47, 140, 153, 255);
	SDL_RenderClear(renderer);
	int imageSize = 384;
	SDL_Rect splashDest = {WIDTH / 2 - imageSize / 2, HEIGHT / 2 - imageSize / 2, imageSize, imageSize};
	SDL_RenderCopy(renderer, loadScreenTex, NULL, &splashDest);
	SDL_RenderPresent(renderer);

	levels = calloc(1, sizeof(LevelComponent));//TEMP

	loadLua();
	NewEntity();
	// lua_close(L);
	INV_WriteCell("set", 4, 2, find_item("stone"));
	INV_WriteCell("set", 6, 99, find_item("feather"));
	INV_WriteCell("set", 2, 24, find_item("wood"));
	INV_WriteCell("set", 1, 17, find_item("nylium"));
	INV_WriteCell("set", 3, 16, find_item("grass"));
	INV_WriteCell("set", 0, 16, find_item("water"));
	INV_WriteCell("set", 12, 17, find_item("flower"));

	droppedItems = malloc(sizeof(DroppedItemComponent) * 2);
	// LoadLevel("maps/map1.dat");
	InitializeBlankLevel(&levels[0], (Vector2){512, 512});
	// InitializeBlankLevel(&levels[0], (Vector2){100, 100});
	GenerateProceduralMap(50, 5);
	activeLevel = &levels[0];
	// LoadLevel("data/maps/testmap.dat");
	// SaveLevel(&levels[0], "data/maps/testMap.dat");
	SaveLevel(activeLevel, "data/maps/testMap.dat");


	// DropItem(find_item("wood"), 1, (Vector2){100, 200});
	// DropItem(find_item("stone"), 1, (Vector2){150, 200});

	// SDL_Rect windowRect = {-tileSize, -tileSize, WIDTH + tileSize, HEIGHT + tileSize};
	SDL_GetWindowSize(window, &WIDTH, &HEIGHT);
	midScreen.x = (WIDTH / 2 - tileSize / 2);
	midScreen.y = (HEIGHT / 2 - tileSize / 2);
	// characterOffset = (fVector2)midScreen;
	characterOffset.x = midScreen.x;
	characterOffset.y = midScreen.y;
	SetupRenderFrame();
	
	
	NewParticleSystem(&pSys1, 1, (SDL_Rect){0, 0, WIDTH, HEIGHT}, 1000, (Range)/*x*/{-1, 1}, (Range)/*y*/{1, 1}, (Range){20, 70});//Snow
	// NewParticleSystem(&pSys1, 2, (SDL_Rect){0, 0, WIDTH, HEIGHT}, 1000, (Range)/*x*/{0, 0}, (Range)/*y*/{5, 6}, (Range){20, 70});//Rain
	pSys1.boundaryCheck = true;
	// pSys1.fade = false;
	pSys1.playSystem = false;
	
	chatHistory = malloc(1 * sizeof(char));


	// character.collider = (CollisionComponent){false, false, false, false};
	// runScript("scripts/init.lua");
	// printf("%s\n", find_tilesheet("character")->name);
	// printf("%s\n", tilesheets[2].name);
	
}

Vector2 tmpSize;	
void ResizeWindow(){
	Vector2 roundSpeed = {mapOffsetPos.x % 4, mapOffsetPos.y % 4};/*Make sure the character position is always a multiple of 4
	keeping everything pixel perfect*/
	if(roundSpeed.x != 0){
		mapOffsetPos.x -= roundSpeed.x;
	}
	if(roundSpeed.y != 0){
		mapOffsetPos.y -= roundSpeed.y;
	}

	clearScreen(renderer);
	SDL_GetWindowSize(window, &WIDTH, &HEIGHT);
	Vector2 diff = {WIDTH - tmpSize.x, HEIGHT - tmpSize.y};
	
	mapOffsetPos.x -= diff.x / 2;
	mapOffsetPos.y -= diff.y / 2;
	
	characterOffset.x = WIDTH / 2 - tileSize / 2;
	characterOffset.y = HEIGHT / 2 - tileSize / 2;
	
	tmpSize = (Vector2){WIDTH, HEIGHT};
	windowRect = (SDL_Rect){-tileSize, -tileSize, WIDTH + tileSize, HEIGHT + tileSize};
}

char currentCollectedText[128] = "";
char consoleOutput[512] = "";
void ParseConsoleCommand(char *command){
	if(command[0] == '/'){
		strcpy(command, strshft_l(command, 1));

		if(strcmp(command, "help") == 0){
			strcpy(consoleOutput, "Possible commands:\ndebug lightcycle hitbox noclip\nreachlimit fullscreen");
		}
		if(strcmp(command, "debug") == 0){
			showDebugInfo = !showDebugInfo;
		}
		if(strcmp(command, "lightcycle") == 0){
			doDayCycle = !doDayCycle;
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
			SDL_DisplayMode gMode;
			SDL_GetDesktopDisplayMode(0, &gMode);
			WIDTH = gMode.w;
			HEIGHT = gMode.h;
			SDL_SetWindowBordered(window, false);
			SDL_SetWindowPosition(window, 0, 0);
			SDL_SetWindowSize(window, WIDTH, HEIGHT);
			ResizeWindow();
		}
	}
}

fVector2 cameraSmooth = {0, 0};

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
			
			float playerVelocity = playerSpeed * deltaTime;
			
			const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
			if(currentKeyStates[SDL_SCANCODE_ESCAPE]){
				quit = true;
			}
			
			isWalking = false;

			if(inputMode == 0){
				if(!(currentKeyStates[SDL_SCANCODE_A] && currentKeyStates[SDL_SCANCODE_D])){
					if(currentKeyStates[SDL_SCANCODE_A]){
						if(!character.collider.colLeft){
							mapOffsetPos.x -= playerVelocity;
							// cameraSmooth.x -= (mapOffsetPos.x + playerVelocity) * 0.01f;
							// mapOffsetPos.x -= cameraSmooth.x;
							isWalking = true;
						}
						characterFacing = 0;
					}
					if(currentKeyStates[SDL_SCANCODE_D]){
						if(!character.collider.colRight){
							mapOffsetPos.x += playerVelocity;
							isWalking = true;
						}
						characterFacing = 1;
					}
				}
				if(!(currentKeyStates[SDL_SCANCODE_W] && currentKeyStates[SDL_SCANCODE_S])){
					if(currentKeyStates[SDL_SCANCODE_W]){
						if(!character.collider.colUp){
							mapOffsetPos.y -= playerVelocity;
							isWalking = true;
						}
					}
					if(currentKeyStates[SDL_SCANCODE_S]){
						if(!character.collider.colDown){
							mapOffsetPos.y += playerVelocity;
							isWalking = true;
						}
					}
				}
					
				if(currentKeyStates[SDL_SCANCODE_Q]){
					characterOffset.x = 0;
					characterOffset.y = 0;
					printf("Character Offset Reset!");
				}
			}

			mouseClicked = false;
			mouseHeld = false;
			if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT) || SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)){
				mouseHeld = true;
			}
			while(SDL_PollEvent(&e) != 0){
				SDL_GetMouseState(&mouseTransform.screenPos.x, &mouseTransform.screenPos.y);
				
				if(e.type == SDL_MOUSEBUTTONDOWN){
					if(e.key.state == SDL_RELEASED){
						mouseClicked = true;
					}
				}
				
				/*if(e.type == SDL_MOUSEBUTTONUP){
					// if(e.key.state = SDL_PRESSED){
						printf("boop\n");
						mouseClicked = false;
					// }
				}*/
				if(inputMode == 1){
					if(e.type == SDL_TEXTINPUT){
						strcat(currentCollectedText, e.text.text);
					}
				}

				if(e.type == SDL_KEYDOWN){//KEY DOWN
					if(inputMode == 0){
						if(e.key.keysym.sym == SDLK_y){
							characterOffset.x = midScreen.x;
							characterOffset.y = midScreen.y;
						}
						if(e.key.keysym.sym >= 49 && e.key.keysym.sym <= 57 && e.key.keysym.sym - 49 < INV_WIDTH){//Hotbar slot selection via number keys
							selectedHotbar = e.key.keysym.sym - 49;
						}
					}else if(inputMode == 1){
						if(e.key.keysym.sym == SDLK_BACKSPACE){
							currentCollectedText[strlen(currentCollectedText) - 1] = '\0';
						}
					}
				}

				if(e.type == SDL_KEYUP){//KEY UP
					Vector2 roundSpeed = {mapOffsetPos.x % 4, mapOffsetPos.y % 4};//Make sure the character position is always a multiple of 4\
					keeping everything pixel perfect
					if(roundSpeed.x != 0){
						mapOffsetPos.x -= roundSpeed.x;
					}
					if(roundSpeed.y != 0){
						mapOffsetPos.y -= roundSpeed.y;
					}
					if(e.key.keysym.sym == SDLK_F3){
						showDebugInfo = !showDebugInfo;
					}
					// if(e.key.keysym.sym == SDLK_F10){//Memory leak possible
					// 	loadLua();
					// }
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
						if(e.key.keysym.sym == SDLK_c){
							character.collider.noClip = !character.collider.noClip;
						}
						if(e.key.keysym.sym == SDLK_x){
							enableHitboxes = !enableHitboxes;
						}
						if(e.key.keysym.sym == SDLK_e){
							showInv = !showInv;
						}
						if(e.key.keysym.sym == SDLK_r){
							// INV_WriteCell("add", INV_FindEmpty(0), 10, 1);
							// TextExtrapolate(levels[0].collision);
						}
						if(e.key.keysym.sym == SDLK_b){//Fullscreen
							SDL_DisplayMode gMode;
							SDL_GetDesktopDisplayMode(0, &gMode);
							WIDTH = gMode.w;
							HEIGHT = gMode.h;
							SDL_SetWindowBordered(window, false);
							SDL_SetWindowPosition(window, 0, 0);
							SDL_SetWindowSize(window, WIDTH, HEIGHT);
							ResizeWindow();
						}
					}
				}

				if(e.type == SDL_MOUSEWHEEL){//SCROLL
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

				if(e.type == SDL_WINDOWEVENT){//WINDOW RESIZE / MINIMIZE
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
		}
	}else{
		printf("Failed to initialize\n");
	}
	
	if(quit){
		Quit();
	}
	return 0;
}

fVector2 entPos = {0, 0};
void RenderScreen(){
	clearScreen(renderer);
	//Call SDL draw functions here and call RenderScreen from the main loop
	
	DrawLevel();

	RenderPauseMenu();
	//Render the player's hitbox
	if(enableHitboxes){
		// AddToRenderQueue(renderer, *find_tilesheet("ui"), 1, charCollider_left, -1, 750);
		// AddToRenderQueue(renderer, *find_tilesheet("ui"), 1, charCollider_right, -1, 750);
		// AddToRenderQueue(renderer, *find_tilesheet("ui"), 1, charCollider_bottom, -1, 750);
		// AddToRenderQueue(renderer, *find_tilesheet("ui"), 1, charCollider_top, -1, 750);
	}

	// Vector2 m = {(mapOffsetPos.x - entPos.x), (mapOffsetPos.y - entPos.y)};
	// int entSpeed = 2;
	// SDL_Rect entRect = {entPos.x, entPos.y, 64, 64};
	// AddToRenderQueue(renderer, *find_tilesheet("items"), 4, entRect, 255, 10000);
	// if(entPos.x > entPos.y){
	// 	entPos.x += entSpeed * m.x / m.y;
	// 	entPos.y += entSpeed;
	// }else if(entPos.y > entPos.x){
	// 	entPos.x += entSpeed;
	// 	entPos.y += entSpeed * m.y / m.x;
	// }else{
	// 	entPos.x += entSpeed;
	// 	entPos.y += entSpeed;
	// }
	RenderDroppedItems();
	
	FindCollisions();
	
	INV_DrawInv();

	char coordinates[256];
	char charoff[256];
	
	// playerCoord = (Vector2){
	character.transform.tilePos = (Vector2){
		((characterOffset.x + 32) + mapOffsetPos.x) / 64,
		((characterOffset.y + 32) + mapOffsetPos.y) / 64,
	};
	
	SDL_Rect mapRect = {-mapOffsetPos.x + 4, -mapOffsetPos.y + 4, 64 * 32 - 8, 64 * 32 - 8};
	pSys1.area = mapRect;
	RenderParticleSystem(pSys1);
	
	RenderConsole();
	
	RenderEntities();

	if(showDebugInfo){
		snprintf(coordinates, 1024, "Player Coordinates ->\nx: %d, y: %d", character.transform.tilePos.x, character.transform.tilePos.y);
		snprintf(charoff, 1024, "MapOffset ->\nx: %d, y: %d", mapOffsetPos.x, mapOffsetPos.y);
		RenderText_d(renderer, coordinates, 0, 0);
		RenderText_d(renderer, charoff, 0, 48);	
		char frameCount[64];
		snprintf(frameCount, 128, "%d RenderCopy calls", renderItemIndex);
		RenderText_d(renderer, frameCount, WIDTH - 200, 0);

		//Performance bar
		AddToRenderQueue(renderer, debugSheet, 4, (SDL_Rect){WIDTH - 10, 10, 10, deltaTime * 20}, 255, 1000);
	}

	SDL_Rect woahR = {0, 0, 100, 100};
	AddToRenderQueue(renderer, colorModSheet, 0, woahR, 0, 1000);
	RenderCursor();
	
	
	RenderUpdate();
	particleCount = 0;
	
	SDL_RenderCopy(renderer, colorModTex, NULL, NULL);
	SDL_RenderPresent(renderer);
}


void RenderTextureInWorld(SDL_Renderer *renderer, WB_Tilesheet sheet, int tile, SDL_Rect rect, int zPos){
	rect.x -= mapOffsetPos.x;
	rect.y -= mapOffsetPos.y;
	AddToRenderQueue(renderer, sheet, tile, rect, -1, zPos);
}

Entity *enemies;
void NewEntity(){
	enemies = (Entity *) malloc(sizeof(Entity));
	//Declare renderer component
	SDL_Rect enemyPos = {50, 50, 64, 64};
	enemies[0].renderer.renderer = renderer;
	enemies[0].renderer.tileSheet = *find_tilesheet("items");
	enemies[0].renderer.tile = 0;
	enemies[0].renderer.transform = enemyPos;
}

void RenderEntities(){
	for(int i = 0; sizeof(enemies) / sizeof(enemies[0]); i++){
		RenderTextureInWorld(enemies[i].renderer.renderer, enemies[i].renderer.tileSheet, enemies[i].renderer.tile, enemies[i].renderer.transform, 10000);
	}

	/*
	// printf("started here\n");
	fVector2 m = {(characterOffset.x + mapOffsetPos.x) - entPos.x, (characterOffset.y + mapOffsetPos.y) - entPos.y};
	int entSpeed = 2;
	// RenderTextureInWorld(renderer, *find_tilesheet("items"), 4, entRect, 10000);
	// if()
	if(abs(m.y) > abs(m.x)){
		entPos.x += entSpeed * ((float)m.x / (float)m.y);//Problem
		entPos.y += entSpeed * ((m.y < 0) ? -1 : 1);
	}else if(abs(m.x) > abs(m.y)){
		entPos.x += entSpeed * ((m.x < 0) ? -1 : 1);
		entPos.y += entSpeed * ((float)m.y / (float)m.x);//Problem
	}else{
		entPos.x += entSpeed * ((m.x < 0) ? -1 : 1);
		entPos.y += entSpeed * ((m.y < 0) ? -1 : 1);
		// entPos.y += entSpeed;
		// entPos.x += entSpeed;
	}
	SDL_Rect entRect = {entPos.x - mapOffsetPos.x, entPos.y - mapOffsetPos.y, 64, 64};
	AddToRenderQueue(renderer, *find_tilesheet("items"), 4, entRect, 255, 10000);
	printf("%f / %f\n", m.x, m.y);
	// printf("%f\n", (float)m.x / (float)m.y);
	// printf("%f / %f\n", entPos.x, entPos.y);
	// printf("%d\n", 12 * (-10 < 0 ? -1 : 1));*/
}

DroppedItemComponent *droppedItems;
int numDroppedItems = 0;

void DropItem(ItemComponent *item, int qty, Vector2 pos){
	if(qty > 0){
		if(levels[0].collision[mouseTransform.tilePos.y][mouseTransform.tilePos.x] != 0){
			droppedItems = realloc(droppedItems, (numDroppedItems + 1) * sizeof(DroppedItemComponent));

			droppedItems[numDroppedItems].item = item;
			droppedItems[numDroppedItems].qty = qty;
			droppedItems[numDroppedItems].transform.worldPos = (Vector2)pos;
			droppedItems[numDroppedItems].animLocation = 0;
			droppedItems[numDroppedItems].animDir = 0;

			numDroppedItems++;
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
			AddToRenderQueue(renderer, droppedItems[i].item->sheet, droppedItems[i].item->tile, itemRect, 255, RNDRLYR_PLAYER - 1);
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


float lerp(float goal, float current, float increment){
	float difference = goal - current;

	if(difference > increment){
		return current + increment;
	}
	if(difference < -increment){
		return current - increment;
	}

	return goal;
}