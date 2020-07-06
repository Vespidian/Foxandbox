#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "headers/DataTypes.h"
#include "headers/ECS.h"
#include "headers/initialize.h"
#include "headers/tileMap.h"
#include "headers/inventory.h"

//The window we'll be rendering to
SDL_Window *gWindow = NULL;
//The window renderer
SDL_Renderer *gRenderer = NULL;

//Initial dimensions of the window
int WIDTH = 960;
int HEIGHT = 960;
bool success = true;


SDL_Texture *undefinedTex;
WB_Tilesheet undefinedSheet;

SDL_Texture *backgroundTex;
WB_Tilesheet backgroundSheet;

SDL_Texture *fontTex;
WB_Tilesheet fontSheet;

SDL_Texture *debugTex;
WB_Tilesheet debugSheet;

SDL_Texture *particleTex;
WB_Tilesheet particleSheet;


SDL_Texture *colorModTex;
WB_Tilesheet colorModSheet;


void TextureInit(){
	undefinedTex = IMG_LoadTexture(gRenderer, "images/undefined.png");
	undefinedSheet = (WB_Tilesheet){"undefined", undefinedTex, 16, 1, 1};

	backgroundTex = IMG_LoadTexture(gRenderer, "images/loadScreen.png");
	
	fontTex = IMG_LoadTexture(gRenderer, "fonts/font.png");
	fontSheet = (WB_Tilesheet){"font", fontTex, 16, 12, 8};
	
	colorModTex = IMG_LoadTexture(gRenderer, "images/singlePixel.png");
	colorModSheet = (WB_Tilesheet){"singlePixel", colorModTex, 16, 1, 1};
	
	debugTex = IMG_LoadTexture(gRenderer, "images/debug.png");
	debugSheet = (WB_Tilesheet){"debug", debugTex, 16, 8, 8};
	
	particleTex = IMG_LoadTexture(gRenderer, "images/particles.png");
	particleSheet = (WB_Tilesheet){"particles", particleTex, 1, 4, 4};
}

void MapInit(){
	LoadMap("maps/testMap_layer0.csv", map);
	LoadMap("maps/testMap_layer1.csv", map1);
	LoadMap("maps/testMap_temp.csv", furnitureMap);
	LoadDataMap("maps/testMap_colliders.csv", colMap);
	
}

void UndefinedInit(){
	undefinedItem = (ItemComponent){"undefined", "", undefinedSheet, 0};
	undefinedBlock = (BlockComponent){&undefinedItem, &undefinedItem, 1, undefinedSheet, 0};
}

bool init(){
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	
	gWindow = SDL_CreateWindow("Explorable World", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	
	SDL_Surface *gIcon = IMG_Load("images/icon.png");
	SDL_SetWindowIcon(gWindow, gIcon);

	UndefinedInit();
	TextureInit();
	MapInit();
	INV_Init();
	memset(customMap, -1, sizeof(customMap));
	
	ReadItemData();
	// printf("test\n");
	
	SDL_SetTextureColorMod(colorModTex, 0, 0, 255);
	SDL_SetTextureAlphaMod(colorModTex, 0);		
	
	return success;
}

void Quit() {
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;
	
	IMG_Quit();
	SDL_Quit();
}