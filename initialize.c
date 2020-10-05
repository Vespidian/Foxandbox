#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "headers/DataTypes.h"
#include "headers/ECS.h"
#include "headers/initialize.h"
#include "headers/level_systems.h"
#include "headers/inventory.h"
#include "headers/lua_systems.h"

//The window we'll be rendering to
SDL_Window *window = NULL;
//The window renderer
SDL_Renderer *renderer = NULL;

//Initial dimensions of the window
int WIDTH = 1344;
int HEIGHT = 960;
bool success = true;

FILE *logFile;

SDL_Texture *undefinedTex;
TilesheetComponent undefinedSheet;

SDL_Texture *loadScreenTex;
SDL_Texture *backgroundTex;

SDL_Texture *fontTex;
TilesheetComponent fontSheet;

SDL_Texture *debugTex;
TilesheetComponent debugSheet;

SDL_Texture *particleTex;
TilesheetComponent particleSheet;

SDL_Texture *autotileMaskTex;
TilesheetComponent autotileMaskSheet;

SDL_Texture *InvertedAutotileMaskTex;
TilesheetComponent InvertedAutotileMaskSheet;

void TextureInit(){
	undefinedTex = IMG_LoadTexture(renderer, "images/undefined.png");
	undefinedSheet = (TilesheetComponent){"undefined", undefinedTex, 16, 1, 1};

	loadScreenTex = IMG_LoadTexture(renderer, "images/loadScreen.png");
	backgroundTex = IMG_LoadTexture(renderer, "images/background.png");
	
	fontTex = IMG_LoadTexture(renderer, "fonts/font.png");
	fontSheet = (TilesheetComponent){"font", fontTex, 16, 12, 8};
	
	debugTex = IMG_LoadTexture(renderer, "images/debug.png");
	debugSheet = (TilesheetComponent){"debug", debugTex, 16, 8, 8};
	
	particleTex = IMG_LoadTexture(renderer, "images/particles.png");
	particleSheet = (TilesheetComponent){"particles", particleTex, 1, 4, 4};

	autotileMaskTex = IMG_LoadTexture(renderer, "images/autotileMask.png");
	autotileMaskSheet = (TilesheetComponent){"autotileMask", autotileMaskTex, 16, 6, 8};

	InvertedAutotileMaskTex = IMG_LoadTexture(renderer, "images/INVERTEDautotileMask.png");
	InvertedAutotileMaskSheet = (TilesheetComponent){"InvertedAutotileMask", autotileMaskTex, 16, 6, 8};

	DebugLog(D_ACT, "Static textures loaded");
}

void UndefinedInit(){
	undefinedItem = (ItemComponent){"undefined", undefinedSheet, 0};
	undefinedBlock = (BlockComponent){&undefinedItem, &undefinedItem, 1, undefinedSheet, 0, false, -1, "terrain"};
	DebugLog(D_ACT, "Initialized undefined variables");
}

bool init(){
	//Set up log file
	logFile = fopen("log.txt", "a");
	fprintf(logFile, "\n---------------\nSEPARATOR\n---------------\n\n");

	//Set up SDL2 and SDL_image
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	window = SDL_CreateWindow("Explorable World", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Surface *gIcon = IMG_Load("images/icon.png");
	SDL_SetWindowIcon(window, gIcon);
	DebugLog(D_ACT, "SDL Initialized");

	UndefinedInit();
	TextureInit();

	INV_Init();

	return success;
}

void Quit() {
	DebugLog(D_ACT, "Shutting down");
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;
	
	fclose(logFile);
	IMG_Quit();
	SDL_Quit();
}

void DebugLog(int type, const char *format, ...){
	va_list vaFormat;

	va_start(vaFormat, format);

	char *formattedText = malloc((strlen(format) + 64) * sizeof(char));
	vsprintf(formattedText, format, vaFormat);

	va_end(vaFormat);

	// time_t rawtime;
	// struct tm *currentTime;
	// time(&rawtime);
	// currentTime = localtime(&rawtime);
	// fprintf(logFile, "%d-%d-%d %d:%d:%d: ", 1900 + currentTime->tm_year, currentTime->tm_mon,
	// currentTime->tm_mday, currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec);

	switch(type){
		case D_ACT:
			fprintf(logFile, "[ACTION]: ");
			break;
		case D_WARN:
			fprintf(logFile, "[WARNING]: ");
			break;
		case D_ERR:
			fprintf(logFile, "[ERROR]: ");
			break;
		case D_SCRIPT_ERR:
			fprintf(logFile, "[LUA_ERROR]: ");
			break;
		case D_SCRIPT_ACT:
			fprintf(logFile, "[LUA_ACTION]: ");
			break;
		default:
			fprintf(logFile, "[UNKNOWN]: ");
			break;
	}

	fprintf(logFile, "%s\n", formattedText);
	free(formattedText);
	fflush(logFile);
}