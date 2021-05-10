#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <SDL.h>
#include <SDL_image.h>

#include "debug.h"

#define DEBUG_BUILD

typedef struct Vector2{
	int x;
	int y;
}Vector2;

typedef struct fVector2{
	float x;
	float y;
}fVector2;

extern bool running;
extern float deltatime;
extern int target_framerate;

void Quit();

#endif