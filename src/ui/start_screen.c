#include <dirent.h>

#include "../global.h"
#include "../render/renderer.h"
#include "start_screen.h"
#include "ui.h"
#include "ui_elements.h"
#include "../world/sandbox.h"

const int dirStringSize = 260;
char **sandboxDirs;
int numSandboxDirs = 0;

int menuIndex = 0;

void ListSandboxes();

void RenderStartScreen(){
    SDL_SetRenderDrawColor(renderer, 139, 214, 239, 255);
	SDL_RenderClear(renderer);

    Vector2 buttonSize = {200, 32};
    Vector2 origin = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    switch(menuIndex){
        case 1:


            for(int i = 0; i < numSandboxDirs; i++){
                if(Button_format(renderer, VerticalRectList(numSandboxDirs + 1, i, buttonSize, origin, 6), sandboxDirs[i])){
                    ReadSandbox(sandboxDirs[i]);
                }
            }
            if(Button(renderer, VerticalRectList(numSandboxDirs + 1, numSandboxDirs + 1, buttonSize, origin, 6), "Back")){menuIndex -= 1;}
            break;

        default:
            if(Button(renderer, VerticalRectList(1, 0, buttonSize, origin, 6), "Saves")){
                menuIndex = 1;
                ListSandboxes();
            }
            break;
    }

    RenderQueue();
	SDL_RenderPresent(renderer);
}

void ListSandboxes(){
    DIR *dir;
    struct dirent *ent;
    if(numSandboxDirs > 0){
        for(int i = 0; i < numSandboxDirs; i++){
            free(sandboxDirs[i]);
        }
    }
    numSandboxDirs = 0;
    if((dir = opendir("../saves/")) != NULL){
        while((ent = readdir(dir)) != NULL){
            if(strlen(ent->d_name) >= 3){
                sandboxDirs = realloc(sandboxDirs, sizeof(char) * (dirStringSize) * (numSandboxDirs + 1));
                sandboxDirs[numSandboxDirs] = malloc(sizeof(char) * dirStringSize);
                strcpy(sandboxDirs[numSandboxDirs], ent->d_name);
                numSandboxDirs++;
            }
        }
        closedir(dir);
    }else{
        DebugLog(D_ERR, "Could not open saves directory");
    }
}