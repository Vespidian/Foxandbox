#ifndef COLLISION_H_
#define COLLISION_H_


extern SDL_Rect charCollider;

void FindCollisions();

extern Vector2 charTilePos;
/*
SDL_Rect setRect(int x, int y, int w, int h);
void CheckCollisions(SDL_Rect tileR);
void ChangeOrder(SDL_Rect tileR, SDL_Rect charCollider, int def, int changed);
*/

#endif