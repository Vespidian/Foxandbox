#ifndef ITEM_H_
#define ITEM_H_

#include "../render/tilesheet.h"

typedef struct ItemObject{
    char *name;
    TilesheetObject *tilesheet;
    int tileIndex;
    bool isBlock;
}ItemObject;


extern ItemObject *items;
extern ItemObject undefinedItem;


void InitItems();
void CreateItem(char *name, TilesheetObject *tilesheet, int tileIndex);
ItemObject *FindItem(char *name);

#endif