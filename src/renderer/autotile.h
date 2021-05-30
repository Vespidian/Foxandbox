#ifndef AUTOTILE_H_
#define AUTOTILE_H_

void InitAutotile();
uint8_t CalculateAutotile(Vector2_i position);
void RenderAutotile(SDL_Rect screen_position, Vector2_i coordinate);

#endif