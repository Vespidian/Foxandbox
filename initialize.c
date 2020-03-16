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


//SDL_Surface *gPng = NULL;
//SDL_Texture *tex = NULL;

SDL_Texture *tileSheetTex = NULL;
SDL_Texture *characterTex = NULL;
void TextureInit(){
	tileSheetTex = IMG_LoadTexture(gRenderer, "images/tileSheet.png");
	characterTex = IMG_LoadTexture(gRenderer, "images/slime.png");
}

void TextureDestroy(){
	SDL_DestroyTexture(tileSheetTex);	
	SDL_DestroyTexture(characterTex);
}

bool init(bool initTTF){
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	if(initTTF){
		TTF_Init();
	}
	gWindow = SDL_CreateWindow("SDL_Template", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	
	// SDL_SetWindowResizable(gWindow, true);

	TextureInit();
	LoadMap("maps/untitled.csv", map);
	LoadMap("maps/untitledCol.csv", colMap);
	
	return success;
}

void Quit() {
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;
	
	TextureDestroy();
	
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}