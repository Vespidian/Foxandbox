#ifndef BLOCK_H_
#define BLOCK_H_

#include "item.h"

typedef struct BlockObject{
    ItemObject *item;
    ItemObject *breakItem;
    TilesheetObject *tilesheet;
    int tileIndex;
    bool allowRotation;
}BlockObject;


extern BlockObject *blocks;
extern BlockObject undefinedBlock;


void InitBlocks();
void CreateBlock(ItemObject *item, ItemObject *breakItem, TilesheetObject *tilesheet, int tileIndex, bool allowRotation);
BlockObject *FindBlock(char *name);

#endif