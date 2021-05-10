#include "../global.h"
#include "renderer.h"
#include "../utility.h"
#include "render_text.h"

FontObject *fonts;
unsigned int num_fonts = 0;
static unsigned int nextID = 0;

FontObject default_font;

Vector2 char_size = {6, 12};
Vector2 padding = {5, 2};

void InitFonts(){
    default_font = *NewRawFont("default_font", "../images/fonts/default_font.png", (Vector2){6, 12}, (Vector2){5, 2});
    DebugLog(D_ACT, "Initialized font subsystem");
}

FontObject *NewFont(char *name, TilesheetObject *tilesheet, Vector2 char_size, Vector2 padding){
    fonts = realloc(fonts, sizeof(FontObject) * (num_fonts + 1));
    fonts[num_fonts].name = malloc(sizeof(char) * strlen(name));
    fonts[num_fonts] = (FontObject){name, nextID, tilesheet->id, char_size, padding};
    DebugLog(D_ACT, "Created font id '%d' name '%s'", nextID, name);
    num_fonts++;
    nextID++;
    return &fonts[num_fonts - 1];
}

FontObject *NewRawFont(char *name, char *path, Vector2 char_size, Vector2 padding){
    return NewFont(name, NewRawTilesheet(name, path, (Vector2){char_size.x + padding.x * 2, char_size.y + padding.y * 2}), char_size, padding);
}

FontObject *FindFont(char *name){
    for(int i = 0; i < num_fonts; i++){
        if(strcmp(fonts[i].name, name) == 0){
            return &fonts[i];
        }
    }
    return &default_font;
}

FontObject *IDFindFont(unsigned int id){
    for(int i = 0; i < num_fonts; i++){
        if(fonts[i].id == id){
            return &fonts[i];
        }
    }
    return &default_font;
}

void RenderText(SDL_Renderer *renderer, FontObject *font, float fontSize, int xPos, int yPos, char *text, ...){
    va_list vaFormat;

    va_start(vaFormat, text);
    char formattedText[256];
    vsprintf(formattedText, text, vaFormat);
    va_end(vaFormat);

    RenderTextEx(renderer, font, fontSize, xPos, yPos, (SDL_Color){255, 255, 255}, -1, formattedText);
}

void RenderTextEx(SDL_Renderer *renderer, FontObject *font, float fontSize, int xPos, int yPos, SDL_Color color, int numCharacters, char *text, ...){
    va_list vaFormat;

    va_start(vaFormat, text);
    char formattedText[256];
    vsprintf(formattedText, text, vaFormat);
    va_end(vaFormat);

    if(numCharacters != -1){
        formattedText[numCharacters] = '\0';
    }

    SDL_Rect dst = {xPos, yPos, IDFindTilesheet(font->tilesheet)->tileSize.x * fontSize, IDFindTilesheet(font->tilesheet)->tileSize.y * fontSize};
    int charValue = 0;
    for(int i = 0; i < strlen(formattedText); i++){
        charValue = (int)formattedText[i] - (int)' ';

        if(charValue >= 0){
            // PushRender_Tilesheet(renderer, IDFindTilesheet(font->tilesheet), charValue, dst, RNDR_TEXT);
            PushRender_TilesheetEx(renderer, IDFindTilesheet(font->tilesheet), charValue, dst, RNDR_TEXT, 0, 255, color);
            dst.x += padding.x * 1.7f * fontSize;
        }else{
            switch(charValue){
                case -22: // NEWLINE (\n)
                    dst.y += (char_size.y + padding.y) * fontSize;
                    dst.x = xPos;
                    break;
                case -23: // TAB
                    dst.x += 8 * fontSize;
                    break;
            }
        }
    }
}