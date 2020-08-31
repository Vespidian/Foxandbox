#ifndef MAPGENERATION_H_
#define MAPGENERATION_H_

int GetSurroundCount(LevelComponent *level, Vector2 tile);

//Basic procedural map generation functions
void RandomMap(RenderTileComponent map[][32], int ratioPercent, int numTypes, ...);
// void SmoothMap();
void SmoothMap(LevelComponent *level, BlockComponent *main, BlockComponent *secondary);
// void AutotileMap(RenderTileComponent map[][32]);
void AutotileMap(RenderTileComponent map[][32], AutotileComponent autotile);

//Generate a random procedural map
void GenerateProceduralMap(int ratioPercent, int smoothSteps);

void DefineCollisions();
void PlaceBlock(Vector2 tile, BlockComponent *block);

//Edit a single tile on a map
void TileMapEdit(RenderTileComponent tileMap[][32], Vector2 pos, BlockComponent *block, bool collide);


extern RenderTileComponent buildLayer_tmp[32][32];
// extern RenderTileComponent levels[0].terrain[32][32];


#endif