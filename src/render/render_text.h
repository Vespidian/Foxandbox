#ifndef RENDER_TEXT_H_
#define RENDER_TEXT_H_

#include "tilesheet.h"

typedef struct FontObject{
    char *name;
    unsigned int id;
    unsigned int tilesheet;
    Vector2 charSize;
    Vector2 padding;
}FontObject;


extern FontObject *fonts;
extern FontObject defaultFont;


void InitFonts();
FontObject *CreateFont(char *name, TilesheetObject *tilesheet, Vector2 charSize, Vector2 padding);
FontObject *CreateRawFont(char *name, char *path, Vector2 charSize, Vector2 padding);
FontObject *FindFont(char *name);
FontObject *IDFindFont(unsigned int id);
void RenderText(SDL_Renderer *renderer, FontObject *font, float fontSize, int xPos, int yPos, char *text, ...);

#endif