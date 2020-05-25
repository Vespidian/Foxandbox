#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL_ttf.h>

#include "headers/DataTypes.h"
#include "headers/ECS.h"

#include "headers/data.h"
#include "headers/initialize.h"
#include "headers/drawFunctions.h"

SDL_Event e;

char *__strtok_r (char *s, const char *delim, char **save_ptr)
{
  char *end;
  if (s == NULL)
    s = *save_ptr;
  if (*s == '\0')
    {
      *save_ptr = s;
      return NULL;
    }
  /* Scan leading delimiters.  */
  s += strspn (s, delim);
  if (*s == '\0')
    {
      *save_ptr = s;
      return NULL;
    }
  /* Find the end of the token.  */
  end = s + strcspn (s, delim);
  if (*end == '\0')
    {
      *save_ptr = end;
      return s;
    }
  /* Terminate the token and make *SAVE_PTR point past it.  */
  *end = '\0';
  *save_ptr = end + 1;
  return s;
}

void SeedLehmer(int worldSeed, int x, int y){
	nLehmer = ((x & 0xFFFF) << 16 | (y & 0xFFFF)) + worldSeed;
}

uint32_t nLehmer = 0;
uint32_t Lehmer32(){
	nLehmer += 0xe120fc15;
	uint32_t tmp;
	tmp = (uint64_t)nLehmer * 0x4a39b70d;
	uint32_t m1 = (tmp >> 16) ^ tmp;
	tmp = (uint64_t)m1 * 0x12fad5c9;
	uint32_t m2 = (tmp >> 16) ^ tmp;
	return m2;
}

int getRnd(int min, int max){
	return(Lehmer32() % (max - min)) + min;
}


bool GetClick(int x1, int y1, int x2, int y2){
	Vector2 mousePos;
	while(SDL_PollEvent(&e) != 0){
		if(e.type == SDL_MOUSEBUTTONDOWN){
			SDL_GetMouseState(&mousePos.x, &mousePos.y);
			if((mousePos.x >= x1 && mousePos.x <= x2) && (mousePos.y >= y1 && mousePos.y <= y2)){
				return true;
			}else{
				return false;
			}
		}
	}
}

int strshft_l(char stringShift[128], int shiftBy){
	if(strlen(stringShift) < shiftBy){
		return 1;
	}
	char tempString[128];
	for(int i = 0; i < strlen(stringShift) - shiftBy + 1; i++){
		tempString[i] = stringShift[i + shiftBy];
		// printf("%c", tempString[i]);
	}
	// tempString[strlen(tempString) - shiftBy + 1] = '\0';
	// printf("%s\n", tempString);
	strcpy(stringShift, tempString);
	// strcpy(stringShift, &stringShift[shiftBy]);
	// stringShift[strlen(stringShift) - shiftBy];
	return 0;
}