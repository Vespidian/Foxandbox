#ifndef COLLISION_H_
#define COLLISION_H_

extern bool colUp;
extern bool colDown;
extern bool colLeft;
extern bool colRight;

extern SDL_Rect charCollider_bottom;
extern SDL_Rect charCollider_right;
extern SDL_Rect charCollider_left;
extern SDL_Rect charCollider_top;

void FindCollisions();

extern Vector2 charTilePos;
/*
SDL_Rect setRect(int x, int y, int w, int h);
void CheckCollisions(SDL_Rect tileR);
void ChangeOrder(SDL_Rect tileR, SDL_Rect charCollider, int def, int changed);
*/

#endif