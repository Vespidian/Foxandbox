#include <dirent.h>
#include "global.h"

SDL_Rect *GetWindowRect(SDL_Window *window){
    static SDL_Rect tmp = {0, 0, 0, 0};
    SDL_GetWindowSize(window, &tmp.w, &tmp.h);
    return &tmp;
}

int ListDirectory(const char *path, char *dst){
	DIR *dr;
	struct dirent *en;
	dr = opendir(path);
	int strSize = 0;
	if(dr){
		while((en = readdir(dr)) != NULL){
			if(strcmp(".", en->d_name) != 0 && strcmp("..", en->d_name) != 0){
				strcat(dst, en->d_name);
				strcat(dst, ":");
				strSize += strlen(en->d_name) + 1;
			}
		}
		closedir(dr);
	}
	return strSize;
}

int GetLineLength(FILE *file){
	int lineLength = 0;
	while(fgetc(file) != '\n' && feof(file)){
		lineLength++;
	}
	fseek(file, -lineLength - 2, SEEK_CUR);
	return lineLength * 8;
}

int strshft_l(char *stringShift, int startIndex, int shiftBy){
	if(strlen(stringShift) - startIndex < shiftBy){
		return -1;
	}
	char *tempString = malloc(strlen(stringShift) * sizeof(char));
	for(int i = startIndex; i < strlen(stringShift) + 1; i++){
		tempString[i] = stringShift[i + shiftBy];
	}
	strcpy(stringShift, tempString);
	free(tempString);
	return 0;
}

bool CompareVector2(Vector2 v1, Vector2 v2){
	if(v1.x == v2.x && v1.y == v2.y){
		return true;
	}
	return false;
}