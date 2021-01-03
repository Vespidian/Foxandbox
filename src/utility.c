#include <dirent.h>
#include "global.h"

SDL_Rect *GetWindowRect(SDL_Window *window){
    static SDL_Rect tmp = {0, 0, 0, 0};
    SDL_GetWindowSize(window, &tmp.w, &tmp.h);
    return &tmp;
}

void ListDirectory(const char *path, char *dst){
	DIR *dr;
	struct dirent *en;
	dr = opendir(path);
	if(dr){
		while((en = readdir(dr)) != NULL){
			if(strcmp(".", en->d_name) != 0 && strcmp("..", en->d_name) != 0){
				strcat(dst, en->d_name);
				strcat(dst, ":");
			}
		}
		closedir(dr);
	}
}