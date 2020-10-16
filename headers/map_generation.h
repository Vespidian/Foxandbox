#ifndef map_generation_H_
#define map_generation_H_

/**
 *  \file
 *  \brief Procedural generation tools and level interaction
 */

/**
 *  Initializes blank level pointer to size
 */
void InitializeBlankLevel(LevelComponent *level, Vector2 size);

/**
 *  \returns An integer between 0 and 8 based on tiles of type type surrounding tile
 */
int GetSurroundCount(LevelComponent *level, Vector2 tile, BlockComponent *type);

/**
 *  Randomly fills terrain layer of level with base and secondary tiles based on ratioPercent
 */
void RandomMap(LevelComponent *level, char *layer, int ratioPercent, BlockComponent *base, BlockComponent *secondary);

/**
 *  Uses cellular automata to order randomness
 */
void SmoothMap(LevelComponent *level, BlockComponent *main, BlockComponent *secondary);

/**
 * Combinations of map_generation functions to generate a procedural map
 */
void GenerateProceduralMap(LevelComponent *level, int ratioPercent, int smoothSteps);

/**
 *  Calculate collision layer
 */
void DefineCollisions(LevelComponent *level);

/**
 *  Replace block at tile position with specified block
 */
void PlaceBlock(Vector2 tile, BlockComponent *block);

#endif