#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <SDL2/SDL.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <luaconf.h>

#include "headers/DataTypes.h"
#include "headers/ECS.h"
#include "headers/data.h"
#include "headers/initialize.h"


SDL_Event e;

char *__strtok_r (char *s, const char *delim, char **save_ptr)
{
  	char *end;
	if (s == NULL)
		s = *save_ptr;
	if (*s == '\0'){
		*save_ptr = s;
		return NULL;
	}
	/* Scan leading delimiters.  */
	s += strspn (s, delim);
	if (*s == '\0'){
		*save_ptr = s;
		return NULL;
	}
	/* Find the end of the token.  */
	end = s + strcspn (s, delim);
	if (*end == '\0'){
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

// char *strshft_l(char stringShift[128], int shiftBy){
int strshft_l(char *stringShift, int shiftBy){
	if(strlen(stringShift) < shiftBy){
		return -1;
	}
	char *tempString = malloc(strlen(stringShift) * sizeof(char));
	for(int i = shiftBy; i < strlen(stringShift) + 1; i++){
		tempString[i] = stringShift[i + shiftBy];
	}
	strcpy(stringShift, tempString);
	// strcpy(stringShift, tempString);
	return 0;
}