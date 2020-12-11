#ifndef BLOCK_H_
#define BLOCK_H_

#include "item.h"

typedef struct BlockObject{
    unsigned int id;
    unsigned int item;
    unsigned int breakItem;
    unsigned int tilesheet;
    int tileIndex;
    bool allowRotation;
}BlockObject;


extern BlockObject *blocks;
extern BlockObject undefinedBlock;


void InitBlocks();
void NewBlock(ItemObject *item, ItemObject *breakItem, TilesheetObject *tilesheet, int tileIndex, bool allowRotation);
BlockObject *FindBlock(char *name);
BlockObject *IDFindBlock(unsigned int id);

#endif