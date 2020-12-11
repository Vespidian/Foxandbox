#include "../global.h"
#include "renderer.h"
#include "../utility.h"
#include "render_text.h"

FontObject *fonts;
unsigned int numFonts = 0;
static unsigned int nextID = 0;

FontObject defaultFont;

Vector2 charSize = {6, 12};
Vector2 padding = {5, 2};

void InitFonts(){
    defaultFont = *NewRawFont("default_font", "../images/fonts/default_font.png", (Vector2){6, 12}, (Vector2){5, 2});
    DebugLog(D_ACT, "Initialized font subsystem");
}

FontObject *NewFont(char *name, TilesheetObject *tilesheet, Vector2 charSize, Vector2 padding){
    fonts = realloc(fonts, sizeof(FontObject) * (numFonts + 1));
    fonts[numFonts].name = malloc(sizeof(char) * strlen(name));
    fonts[numFonts] = (FontObject){name, nextID, tilesheet->id, charSize, padding};
    DebugLog(D_ACT, "Created font id '%d' name '%s'", nextID, name);
    numFonts++;
    nextID++;
    return &fonts[numFonts - 1];
}

FontObject *NewRawFont(char *name, char *path, Vector2 charSize, Vector2 padding){
    return NewFont(name, NewRawTilesheet(name, path, (Vector2){charSize.x + padding.x * 2, charSize.y + padding.y * 2}), charSize, padding);
}

FontObject *FindFont(char *name){
    for(int i = 0; i < numFonts; i++){
        if(strcmp(fonts[i].name, name) == 0){
            return &fonts[i];
        }
    }
    return &defaultFont;
}

FontObject *IDFindFont(unsigned int id){
    for(int i = 0; i < numFonts; i++){
        if(fonts[i].id == id){
            return &fonts[i];
        }
    }
    return &defaultFont;
}

void RenderText(SDL_Renderer *renderer, FontObject *font, float fontSize, int xPos, int yPos, char *text, ...){
    va_list vaFormat;

    va_start(vaFormat, text);
    char formattedText[256];
    vsprintf(formattedText, text, vaFormat);
    va_end(vaFormat);


    SDL_Rect dst = {xPos, yPos, IDFindTilesheet(font->tilesheet)->tileSize.x * fontSize, IDFindTilesheet(font->tilesheet)->tileSize.y * fontSize};
    int charValue = 0;
    for(int i = 0; i < strlen(formattedText); i++){
        charValue = (int)formattedText[i] - (int)' ';

        if(charValue >= 0){
            PushRender_Tilesheet(renderer, IDFindTilesheet(font->tilesheet), charValue, dst, RNDR_TEXT);
            dst.x += padding.x * 1.7f * fontSize;
        }else{
            switch(charValue){
                case -22: // NEWLINE (\n)
                    dst.y += (charSize.y + padding.y) * fontSize;
                    dst.x = xPos;
                    break;
                case -23: // TAB
                    dst.x += 8 * fontSize;
                    break;
            }
        }
    }
}