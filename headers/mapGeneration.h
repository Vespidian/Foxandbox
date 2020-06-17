#ifndef MAPGENERATION_H_
#define MAPGENERATION_H_

int GetSurroundCount(Vector2 tile);

//Basic procedural map generation functions
void RandomMap(int ratioPercent);
void SmoothMap();
void AutotileMap(RenderTileComponent map[][32]);

//Generate a random procedural map
void GenerateProceduralMap(int ratioPercent, int smoothSteps);

//Edit a single tile on a map
void TileMapEdit(RenderTileComponent tileMap[][32], Vector2 pos, int tile, bool collide);


extern RenderTileComponent buildLayer_tmp[32][32];
extern RenderTileComponent buildLayer[32][32];


#endif