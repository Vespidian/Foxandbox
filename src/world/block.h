#ifndef BLOCK_H_
#define BLOCK_H_

#include "../entities/item.h"

typedef struct BlockObject{
    unsigned int id;
    unsigned int item;
    unsigned int breakItem;
    unsigned int tilesheet;
    int tileIndex;
    bool allow_rotation;
    bool allow_autotile;
}BlockObject;


extern BlockObject *blocks;
extern BlockObject undefined_block;


void InitBlocks();
void NewBlock(ItemObject *item, ItemObject *breakItem, TilesheetObject *tilesheet, int tileIndex, bool allow_rotation);
BlockObject *FindBlock(char *name);
BlockObject *IDFindBlock(unsigned int id);

#endif