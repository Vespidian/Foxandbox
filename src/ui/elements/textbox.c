#include "../../global.h"
#include "../../render/renderer.h"
#include "../../render/render_text.h"
#include "../../event.h"
#include "../../text_event.h"
#include "../resizable_rect.h"

#include "textbox.h"

void Textbox_f(Textbox_t *box){
    if(mouseClicked){
        //if mouse hovered over and clicked, textbox is selected 
        if(SDL_PointInRect(&mousePos, &box->pos)){
            if(box->text == NULL){
                box->text = malloc(sizeof(char) * 2);
                box->text[0] = '\0';
            }
            textBuffer = calloc((strlen(box->text) + 1), sizeof(char));//here
            textBufferSize = strlen(box->text) + 1;
            strcpy(textBuffer, box->text);
            box->selected = true;
        }else{//if mouse clicks elsewhere, textbox no longer selected
            box->selected = false;
            textInput = false;
        }
    }
    if(box->selected){
        textInput = true;
        if(textBuffer != NULL){
            if(strcmp(textBuffer, box->text) != 0){
                box->text = realloc(box->text, sizeof(char) * (strlen(textBuffer) + 1));
                strcpy(box->text, textBuffer);
            }
        }
    }
    // render elements
    if(box->text != NULL){
        char *scrolledText = box->text;
        int maxShownChars = (box->pos.w + box->pos.w / 14) / 9;
        if(9 * strlen(box->text) > box->pos.w + box->pos.w / 14){
            int offset = ((strlen(box->text) * 9) - (box->pos.w + box->pos.w / 14)) / 9;
            if(box->selected){// Set text offset to "scroll" the text, not letting it leave the confines of the box
                scrolledText = (box->text + offset);
            }
        }
        RenderTextEx(renderer, FindFont("default_font"), 1, box->pos.x + 2, box->pos.y + box->pos.h / 2 - 8, (SDL_Color){255, 255, 255}, maxShownChars, "%s", scrolledText);
    }
    if(box->selected){
        ResizableRect(box->pos, 9);
    }else{
        ResizableRect(box->pos, 8);
    }
}
