#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include <SDL2/SDL.h>
// #include <SDL_ttf.h>
#include <SDL_image.h>
// #include <SDL_net.h>

#include "headers/DataTypes.h"
#include "headers/ECS.h"
#include "headers/initialize.h"
#include "headers/data.h"
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

SDL_Texture *tileSheetTex;
WB_Tilesheet defSheet;

SDL_Texture *furnitureTex;
WB_Tilesheet furnitureSheet;

SDL_Texture *characterTex;
WB_Tilesheet characterSheet;

SDL_Texture *backgroundTex;
WB_Tilesheet backgroundSheet;

SDL_Texture *itemTex;
WB_Tilesheet itemSheet;

SDL_Texture *uiTex;
WB_Tilesheet uiSheet;

SDL_Texture *fontTex;
WB_Tilesheet fontSheet;

SDL_Texture *debugTex;
WB_Tilesheet debugSheet;

SDL_Texture *particleTex;
WB_Tilesheet particleSheet;


SDL_Texture *colorModTex;
WB_Tilesheet colorModSheet;


void TextureInit(){
	// tileSheetTex = IMG_LoadTexture(gRenderer, "images/groundTiles.png");
	tileSheetTex = IMG_LoadTexture(gRenderer, "images/AUTOTILE_grass-water.png");
	defSheet = (WB_Tilesheet){tileSheetTex, 6, 8, 16};
	
	furnitureTex = IMG_LoadTexture(gRenderer, "images/furniture.png");
	furnitureSheet = (WB_Tilesheet){furnitureTex, 8, 8, 16};
	
	characterTex = IMG_LoadTexture(gRenderer, "images/foxanim.png");
	characterSheet = (WB_Tilesheet){characterTex, 6, 4, 16};
	
	backgroundTex = IMG_LoadTexture(gRenderer, "images/loadScreen.png");
	
	itemTex = IMG_LoadTexture(gRenderer, "images/items.png");
	itemSheet = (WB_Tilesheet){itemTex, 8, 16, 16};
	
	uiTex = IMG_LoadTexture(gRenderer, "images/ui.png");
	uiSheet = (WB_Tilesheet){uiTex, 8, 16, 16};
	
	fontTex = IMG_LoadTexture(gRenderer, "fonts/font.png");
	fontSheet = (WB_Tilesheet){fontTex, 12, 8, 16};
	
	colorModTex = IMG_LoadTexture(gRenderer, "images/singlePixel.png");
	colorModSheet = (WB_Tilesheet){colorModTex, 1, 1, 16};
	
	debugTex = IMG_LoadTexture(gRenderer, "images/debug.png");
	debugSheet = (WB_Tilesheet){debugTex, 8, 8, 16};
	
	particleTex = IMG_LoadTexture(gRenderer, "images/particles.png");
	particleSheet = (WB_Tilesheet){particleTex, 4, 4, 1};
}

void MapInit(){
	LoadMap("maps/testMap_layer0.csv", map);
	LoadMap("maps/testMap_layer1.csv", map1);
	LoadMap("maps/testMap_temp.csv", furnitureMap);
	LoadDataMap("maps/testMap_colliders.csv", colMap);
	
}

bool init(){
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	
	gWindow = SDL_CreateWindow("Explorable World", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	
	SDL_Surface *gIcon = IMG_Load("images/icon.png");
	SDL_SetWindowIcon(gWindow, gIcon);

	TextureInit();
	MapInit();
	INV_Init();
	memset(customMap, -1, sizeof(customMap));
	
	ReadItemData();
	
	SDL_SetTextureColorMod(colorModTex, 0, 0, 255);
	SDL_SetTextureAlphaMod(colorModTex, 0);		
	
	return success;
}

void TextureDestroy(){
	SDL_DestroyTexture(tileSheetTex);	
	SDL_DestroyTexture(furnitureTex);	
	SDL_DestroyTexture(characterTex);
}

void Quit() {
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;
	
	TextureDestroy();
	
	IMG_Quit();
	SDL_Quit();
}