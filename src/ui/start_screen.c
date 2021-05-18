#include <dirent.h>

#include "../global.h"
#include "../renderer/renderer.h"
#include "../world/sandbox.h"
#include "ui.h"
#include "elements/button.h"
#include "elements/textbox.h"

#include "start_screen.h"

const int dirStringSize = 260;
char **sandboxDirs;
int numSandboxDirs = 0;

int menuIndex = 0;

void ListSandboxes();

Textbox_t newWorldInput;

void RenderStartScreen(){
    // SDL_SetRenderDrawColor(renderer, 139, 214, 239, 255);
	// SDL_RenderClear(renderer);


    Vector2 buttonSize = {200, 32};
    Vector2 origin = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    switch(menuIndex){
        case 1:// Play
            for(int i = 0; i < numSandboxDirs; i++){
                if(Button_format(VerticalRectList(numSandboxDirs + 3, i, buttonSize, origin, 6), sandboxDirs[i])){
                    ReadSandbox(sandboxDirs[i]);
                }
            }
            newWorldInput.pos = VerticalRectList(numSandboxDirs + 3, numSandboxDirs, buttonSize, origin, 6);
            Textbox_f(&newWorldInput);
            if(Button(VerticalRectList(numSandboxDirs + 3, numSandboxDirs + 1, buttonSize, origin, 6), "Create World")){
                if(newWorldInput.text != NULL){
                    if(strlen(newWorldInput.text) > 2){
                        ReadSandbox(newWorldInput.text);
                    }
                }
            }
            if(Button(VerticalRectList(numSandboxDirs + 3, numSandboxDirs + 2, buttonSize, origin, 6), "Back")){menuIndex = 0;}
            break;


        case 2:// Options

            if(Button(VerticalRectList(1, 0, buttonSize, origin, 6), "Back")){menuIndex = 0;}
            break;

            
        default:
            if(Button(VerticalRectList(3, 0, buttonSize, origin, 6), "Play")){
                menuIndex = 1;
                ListSandboxes();
            }
            if(Button(VerticalRectList(3, 1, buttonSize, origin, 6), "Options")){menuIndex = 2;}
            if(Button(VerticalRectList(3, 2, buttonSize, origin, 6), "Quit")){Quit();}
            break;
    }

    // RenderQueue();
	// SDL_RenderPresent(renderer);
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