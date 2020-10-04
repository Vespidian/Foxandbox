#ifndef level_systems_H_
#define level_systems_H_

extern const int MAXLEVELSIZE;

extern LevelComponent *activeLevel;

extern LevelComponent *levels;
extern int numLevels;

void RenderLevel(LevelComponent *level);

void DrawLevel();
int UnloadLevel(LevelComponent *level);

int LoadLevel(char *path);
int SaveLevel(LevelComponent *level, char *path);

#endif