#include <math.h>
#include <SDL2/SDL.h>
#include "headers/DataTypes.h"
#include "headers/initialize.h"
// #include "headers/data.h"
#include "headers/drawFunctions.h"

// Function for circle-generation 
// using Bresenham's algorithm 
void DrawCircle(SDL_Renderer *renderer, int xc, int yc, int r)
{ 
    int x = 0, y = r;
    int d = 3 - 2 * r;
    SDL_RenderDrawPoint(renderer, xc+x, yc+y);
    SDL_RenderDrawPoint(renderer, xc-x, yc+y);
    SDL_RenderDrawPoint(renderer, xc+x, yc-y);
    SDL_RenderDrawPoint(renderer, xc-x, yc-y);
    SDL_RenderDrawPoint(renderer, xc+y, yc+x);
    SDL_RenderDrawPoint(renderer, xc-y, yc+x);
    SDL_RenderDrawPoint(renderer, xc+y, yc-x);
    SDL_RenderDrawPoint(renderer, xc-y, yc-x);
    while (y >= x)
    {
        x++;
        if (d > 0) 
        { 
            y--;  
            d = d + 4 * (x - y) + 10; 
        } 
        else
            d = d + 4 * x + 6; 
        SDL_RenderDrawPoint(renderer, xc+x, yc+y);
		SDL_RenderDrawPoint(renderer, xc-x, yc+y);
		SDL_RenderDrawPoint(renderer, xc+x, yc-y);
		SDL_RenderDrawPoint(renderer, xc-x, yc-y);
		SDL_RenderDrawPoint(renderer, xc+y, yc+x);
		SDL_RenderDrawPoint(renderer, xc-y, yc+x);
		SDL_RenderDrawPoint(renderer, xc+y, yc-x);
		SDL_RenderDrawPoint(renderer, xc-y, yc-x);
    } 
}