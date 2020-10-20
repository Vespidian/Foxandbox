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

TilesheetComponent undefinedSheet;

SDL_Texture *loadScreenTex;
SDL_Texture *backgroundTex;

TilesheetComponent fontSheet;

TilesheetComponent debugSheet;

TilesheetComponent particleSheet;

void TextureInit(){
	undefinedSheet = (TilesheetComponent){"undefined", IMG_LoadTexture(renderer, "images/undefined.png"), 16, 1, 1};

	loadScreenTex = IMG_LoadTexture(renderer, "images/loadScreen.png");
	backgroundTex = IMG_LoadTexture(renderer, "images/background.png");
	
	fontSheet = (TilesheetComponent){"font", IMG_LoadTexture(renderer, "fonts/font.png"), 16, 12, 8};
	
	debugSheet = (TilesheetComponent){"debug", IMG_LoadTexture(renderer, "images/debug.png"), 16, 8, 8};
	
	particleSheet = (TilesheetComponent){"particles", IMG_LoadTexture(renderer, "images/particles.png"), 1, 4, 4};

	DebugLog(D_ACT, "Static textures loaded");
}

void UndefinedInit(){
	undefinedItem = (ItemComponent){"undefined", &undefinedSheet, 0};
	undefinedBlock = (BlockComponent){&undefinedItem, &undefinedItem, 1, &undefinedSheet, 0, false, -1, "terrain"};
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



	/*int arraySize = 10;
	int start = 5;
	int *test;


	test = malloc(sizeof(int) * arraySize);
	for(int i = 0; i < arraySize; i++){
		test[i] = rand() % 10;
		printf("%d\n", test[i]);
	}
	printf("\n\n");

	//Important part
	arraySize--;
	for(int i = start; i < arraySize; i++){
		test[i] = test[i + 1];
	}
	test = realloc(test, sizeof(int) * arraySize);
	//Important part

	for(int i = 0; i < arraySize; i++){
		printf("%d\n", test[i]);
	}
	free(test);*/

	return success;
}

void DestroyBlocks(){
	free(blockData);
}

void DestroyItems(){
	for(int i = 0; i < numItems; i++){
		free(itemData[i].name);
	}
	free(itemData);
}

void DestroyTilesheets(){
	for(int i = 0; i < num_tilesheets; i++){
		SDL_DestroyTexture(tilesheets[i].tex);
	}
	free(tilesheets);
}

void Quit() {
	DebugLog(D_ACT, "Shutting down");

	DestroyBlocks();
	DestroyItems();
	DestroyTilesheets();
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