#ifndef UI_H_
#define UI_H_

void InitUI();

/**
 *  Returns the position of a rectangular element in a list of specified size and spacing
 */
SDL_Rect VerticalRectList(int numItems, int itemIndex, Vector2 size, Vector2 origin, int spacing);

/**
 *  Renders a background to a set of VerticalRectList() parameters
 */
void DrawVListBackground(int numItems, Vector2 size, Vector2 origin, int spacing, uint8_t opacity);

#endif