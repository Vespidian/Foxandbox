#ifndef TEXTBOX_H_
#define TEXTBOX_H_

typedef struct Textbox_t{
    char *text;
    char *defaultText;
    bool selected;
    SDL_Rect pos;
}Textbox_t;

void Textbox_f(Textbox_t *box);

#endif