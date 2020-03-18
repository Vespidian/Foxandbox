#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include "headers/DataTypes.h"
#include "headers/initialize.h"
#include "headers/data.h"
#include "headers/drawFunctions.h"
#include "headers/tileMap.h"

//SDL DEFINITIONS
//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

int WIDTH = 960;
int HEIGHT = 960;
bool success = true;

TTF_Font *font = NULL;

SDL_Texture *tileSheetTex = NULL;
SDL_Texture *furnitureTex = NULL;
SDL_Texture *characterTex = NULL;
void TextureInit(){
	tileSheetTex = IMG_LoadTexture(gRenderer, "images/stardewDesert.png");
	furnitureTex = IMG_LoadTexture(gRenderer, "images/furniture.png");
	characterTex = IMG_LoadTexture(gRenderer, "images/slime.png");
}

void MapInit(){
	LoadMap("maps/testMap_layer0.csv", map);
	LoadMap("maps/testMap_layer1.csv", map1);
	LoadMap("maps/testMap_colliders.csv", colMap);
	// LoadMap("maps/testMap_features-collidable.csv", furnitureMap);
	// LoadMap("maps/testMap_features-passable.csv", passableMap);
	
	
	// printf("\n\n%d\n\n", 15 / 4);
	ExtrapolateMap("maps/testMap_temp.csv", furnitureMap, passableMap);
	// TextExtrapolate(tempMap1);
	// printf("\n\n");
	// TextExtrapolate(tempMap2);
	// TextExtrapolate(map);
}

void TextureDestroy(){
	SDL_DestroyTexture(tileSheetTex);	
	SDL_DestroyTexture(furnitureTex);	
	SDL_DestroyTexture(characterTex);
}

bool init(bool initTTF){
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	// if(initTTF){
		// TTF_Init();
	// }
	gWindow = SDL_CreateWindow("SDL_Template", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	
	// SDL_SetWindowResizable(gWindow, true);

	TextureInit();
	MapInit();
	// LoadMap("maps/untitled_features-passable.csv", passableMap);
	// TTF_Font *font = TTF_OpenFont("fonts/font.ttf", 12);
	
	return success;
}

void Quit() {
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;
	
	TextureDestroy();
	// TTF_CloseFont(font);
	
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}