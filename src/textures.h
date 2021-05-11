#ifndef TEXTURES_H_
#define TEXTURES_H_

#include "gl_utils.h"

/**
 *  Array of textures loaded via 'LoadTextureToStack()'
 */
extern TextureObject *texture_stack;

/**
 *  Texture to be used in error cases
 */
extern TextureObject undefined_texture;

/**
 *  @brief Initialize the texture subsystem
 */
void InitTextures();

/**
 *  @brief Load a texture from file and place it into the texture stack
 *  @return A 'TextureObject' containing the texture
 */
TextureObject LoadTextureToStack(const char *path);

/**
 *  @return The texture with id 'id' from the texture stack
 */
TextureObject FindTexture(unsigned int texture);
#endif