#ifndef ITEM_H_
#define ITEM_H_

#include "../render/tilesheet.h"

typedef struct ItemObject{
    char *name;
    unsigned int id;
    unsigned int tilesheet;
    int tileIndex;
    bool isBlock;
}ItemObject;


extern ItemObject *items;
extern ItemObject undefinedItem;


void InitItems();
ItemObject *NewItem(char *name, TilesheetObject *tilesheet, int tileIndex);
ItemObject *FindItem(char *name);
ItemObject *IDFindItem(unsigned int id);

#endif