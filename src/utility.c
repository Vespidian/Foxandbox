#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "global.h"

SDL_Rect *GetWindowRect(SDL_Window *window){
    static SDL_Rect tmp = {0, 0, 0, 0};
    SDL_GetWindowSize(window, &tmp.w, &tmp.h);
    return &tmp;
}