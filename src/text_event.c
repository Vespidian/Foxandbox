#include "global.h"
#include "event.h"

#include "text_event.h"

bool text_input = false;
char *text_buffer;
int text_buffer_size = 0;

void Backspace(EventData event);

void InitTextEvent(){
    BindEvent(EV_ACCURATE, SDL_KEYDOWN, Backspace);
}

void PollText(SDL_Event *event){
	while(SDL_PollEvent(event)){
		if(event->type == SDL_TEXTINPUT){
			text_buffer_size++;
			text_buffer = realloc(text_buffer, sizeof(char) * (text_buffer_size + 1));
			strcat(text_buffer, event->text.text);
            text_buffer[text_buffer_size + 1] = '\0';
		}
	}
}

void Emptytext_buffer(){
    text_buffer = NULL;
    text_buffer_size = 0;
}

void Backspace(EventData event){
    if(text_input){
        if(event.e->key.keysym.sym == SDLK_BACKSPACE){
            if(text_buffer_size > 1){
                text_buffer_size--;
                text_buffer = realloc(text_buffer, sizeof(char) * (text_buffer_size + 1));
                text_buffer[strlen(text_buffer) - 1] = '\0';
            }
        }
    }
}