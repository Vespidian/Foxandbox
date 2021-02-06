#include "global.h"
#include "event.h"

#include "text_event.h"

bool textInput = false;
char *textBuffer;
int textBufferSize = 0;

void Backspace(EventData event);

void InitTextEvent(){
    NewEvent(EV_ACCURATE, SDL_KEYDOWN, Backspace);
}

void PollText(SDL_Event *event){
    // printf("here\n");
	while(SDL_PollEvent(event)){
		if(event->type == SDL_TEXTINPUT){
			textBuffer = realloc(textBuffer, sizeof(char) * (textBufferSize + 1));
			textBufferSize++;
			strcat(textBuffer, event->text.text);
		}
        if(event->type == SDL_KEYDOWN){
            if(event->key.keysym.sym == SDLK_BACKSPACE){
                printf("here\n");
                if(textBufferSize > 0){
                    printf("%s\n", textBuffer);
                    // textBuffer[strlen(textBuffer) - 1] = '\0';
                    // textBufferSize--;
                    // textBuffer = realloc(textBuffer, sizeof(char) * (textBufferSize + 1));
                    printf("%s\n", textBuffer);
                }
            }
        }
        // if(e.key.keysym.sym == SDLK_RETURN){
            // EmptyTextBuffer();
        // }
	}
}

void EmptyTextBuffer(){
    textBuffer = NULL;
    textBufferSize = 0;
    // textBuffer = malloc(sizeof(char));
    // textBuffer = '\0';
    // free(textBuffer);
}

void Backspace(EventData event){
    if(textInput){
        if(event.e->key.keysym.sym == SDLK_BACKSPACE){
            if(textBufferSize > 1){
                printf("backspace!\n");
                textBuffer[strlen(textBuffer) - 1] = '\0';
                textBufferSize--;
                textBuffer = realloc(textBuffer, sizeof(char) * (textBufferSize + 1));
            }
        }
    }
}