#include "../global.h"
#include "../renderer/renderer.h"

#include "resizable_rect.h"

Vector2 tilesheetSize;
Vector2 tileSize;

const SDL_Rect regions[3][3] = {
    {{0, 0, 6, 6}, {6, 0, 4, 6}, {10, 0, 6, 6}},
    {{0, 6, 6, 4}, {6, 6, 4, 4}, {10, 6, 6, 4}},
    {{0, 10, 6, 6}, {6, 10, 4, 6}, {10, 10, 6, 6}},
};

void InitResizableRect(){
    tileSize = FindTilesheet("ui")->tileSize;
    tilesheetSize = (Vector2){
        FindTexture("ui")->w / tileSize.x, 
        FindTexture("ui")->h / tileSize.y
    };
}

void AddRectOffset(SDL_Rect *rect, Vector2 offset){
    rect->x += offset.x;
    rect->y += offset.y;
}

void ResizableRect(SDL_Rect position, int index){
    Vector2 offset = (Vector2){
        (index % tilesheetSize.x) * tileSize.x, 
        (index / tilesheetSize.x) * tileSize.y
    };

    SDL_Rect src;
    SDL_Rect dst = {position.x, position.y, 6, 6};
    for(int y = 0; y < 3; y++){
        if(y == 1){//Middle row
            src = regions[y][0];
            AddRectOffset(&src, offset);
            dst = (SDL_Rect){position.x, dst.y, 6, position.h - 12};
            PushRender(renderer, FindTexture("ui"), src, dst, RNDR_UI);

            src = regions[y][1];
            AddRectOffset(&src, offset);
            dst = (SDL_Rect){position.x + 6, position.y + 6, position.w - 12, position.h - 12};//Center
            PushRender(renderer, FindTexture("ui"), src, dst, RNDR_UI);

            src = regions[y][2];
            AddRectOffset(&src, offset);
            dst = (SDL_Rect){dst.x + position.w - 12, dst.y, 6, position.h - 12};
            PushRender(renderer, FindTexture("ui"), src, dst, RNDR_UI);


            dst.y += position.h - 12;
        }else{
            src = regions[y][0];
            AddRectOffset(&src, offset);
            dst = (SDL_Rect){position.x, dst.y, 6, 6};
            PushRender(renderer, FindTexture("ui"), src, dst, RNDR_UI);

            src = regions[y][1];
            AddRectOffset(&src, offset);
            dst = (SDL_Rect){dst.x + dst.w, dst.y, position.w - 12, 6};
            PushRender(renderer, FindTexture("ui"), src, dst, RNDR_UI);

            src = regions[y][2];
            AddRectOffset(&src, offset);
            dst = (SDL_Rect){dst.x + position.w - 12, dst.y, 6, 6};
            PushRender(renderer, FindTexture("ui"), src, dst, RNDR_UI);


            dst.y += 6;
        }
    }
}