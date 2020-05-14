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
#include "headers/initialize.h"
#include "headers/data.h"
#include "headers/drawFunctions.h"
#include "headers/tileMap.h"
#include "headers/inventory.h"

//SDL DEFINITIONS
//The window we'll be rendering to
SDL_Window *gWindow = NULL;
// SDL_Window* testWindow = NULL;

//The window renderer
SDL_Renderer *gRenderer = NULL;

int WIDTH = 960;
int HEIGHT = 960;
bool success = true;

// TTF_Font *font = NULL;

SDL_Texture *tileSheetTex;
SDL_Texture *furnitureTex;
SDL_Texture *characterTex;
SDL_Texture *backgroundTex;
SDL_Texture *itemTex;
SDL_Texture *uiTex;
SDL_Texture *fontTex;

SDL_Texture *colorModTex;
WB_Tilesheet colorModSheet;

WB_Tilesheet defSheet;
WB_Tilesheet furnitureSheet;
WB_Tilesheet characterSheet;
WB_Tilesheet backgroundSheet;
WB_Tilesheet itemSheet;
WB_Tilesheet uiSheet;
WB_Tilesheet fontSheet;


void TextureInit(){
	tileSheetTex = IMG_LoadTexture(gRenderer, "images/groundTiles.png");
	defSheet = (WB_Tilesheet){tileSheetTex, 16, 16, 16};
	
	furnitureTex = IMG_LoadTexture(gRenderer, "images/furniture.png");
	furnitureSheet = (WB_Tilesheet){furnitureTex, 8, 8, 16};
	
	characterTex = IMG_LoadTexture(gRenderer, "images/character.png");
	characterSheet = (WB_Tilesheet){characterTex, 4, 6, 16};
	
	backgroundTex = IMG_LoadTexture(gRenderer, "images/background.png");
	
	itemTex = IMG_LoadTexture(gRenderer, "images/items.png");
	itemSheet = (WB_Tilesheet){itemTex, 8, 16, 16};
	
	uiTex = IMG_LoadTexture(gRenderer, "images/ui.png");
	uiSheet = (WB_Tilesheet){uiTex, 8, 16, 16};
	
	fontTex = IMG_LoadTexture(gRenderer, "fonts/font.png");
	fontSheet = (WB_Tilesheet){fontTex, 12, 8, 16};
	
	colorModTex = IMG_LoadTexture(gRenderer, "images/singlePixel.png");
	colorModSheet = (WB_Tilesheet){colorModTex, 1, 1, 16};
}

void MapInit(){
	LoadMap("maps/testMap_layer0.csv", map);
	LoadMap("maps/testMap_layer1.csv", map1);
	LoadMap("maps/testMap_colliders.csv", colMap);
		
	ExtrapolateMap("maps/testMap_temp.csv", furnitureMap, passableMap);
}

bool init(bool initTTF){
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	// if(initTTF){
	// TTF_Init();
	// }
	
	gWindow = SDL_CreateWindow("Explorable World", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	
	// SDL_SetWindowResizable(gWindow, true);
	SDL_Surface *gIcon = IMG_Load("images/icon.png");
	SDL_SetWindowIcon(gWindow, gIcon);

	TextureInit();
	MapInit();
	INV_Init();
	memset(customMap, -1, sizeof(customMap));
	// printf("%d\n", invArray[16][0]);
	
	// PerlinInit();
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
	// TTF_CloseFont(font);
	
	IMG_Quit();
	// TTF_Quit();
	SDL_Quit();
}