#ifndef QUAD_H_
#define QUAD_H_

/**
 *  Initialize the quad render system
 */
void InitQuadRender();

/**
 *  Render a quad with a texture to the viewport
 */
void RenderQuad(TextureObject texture, SDL_Rect *src, SDL_Rect *dst, int zpos, SDL_Color color, float rot);

/**
 *  Render a tile from a tilesheet to the viewport
 */
void TilesheetRender(TilesheetObject tilesheet, unsigned int index, SDL_Rect dest, int zpos, SDL_Color color);

#endif