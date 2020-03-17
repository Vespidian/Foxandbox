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

#endif