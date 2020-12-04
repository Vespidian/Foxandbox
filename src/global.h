#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "debug.h"

typedef struct Vector2{
	int x;
	int y;
}Vector2;

extern int targetFramerate;
extern float deltatime;
extern bool running;

#endif