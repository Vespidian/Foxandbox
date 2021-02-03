#include "../global.h"
#include "block.h"

BlockObject *blocks;
unsigned int numBlocks = 0;
static unsigned int nextID = 0;

BlockObject undefinedBlock;

void InitBlocks(){
    undefinedBlock = (BlockObject){-1, undefinedItem.id, undefinedItem.id, undefinedTilesheet.id, 0, false};
    DebugLog(D_ACT, "Initialized block subsystem");
}

void NewBlock(ItemObject *item, ItemObject *breakItem, TilesheetObject *tilesheet, int tileIndex, bool allowRotation){
    blocks = realloc(blocks, sizeof(BlockObject) * (numBlocks + 1));
    if(breakItem == NULL){
        breakItem = item;
    }
    blocks[numBlocks] = (BlockObject){nextID, item->id, breakItem == NULL ? -1 : breakItem->id, tilesheet->id, tileIndex, allowRotation};
    DebugLog(D_ACT, "Created block id '%d' with item name '%s'", numBlocks, item->name);
    numBlocks++;
    nextID++;
}

BlockObject *FindBlock(char *name){
    for(int i = 0; i < numBlocks; i++){
        if(strcmp(IDFindItem(blocks[i].item)->name, name) == 0){
            return &blocks[i];
        }
    }
    return &undefinedBlock;
}

BlockObject *IDFindBlock(unsigned int id){
    for(int i = 0; i < numBlocks; i++){
        if(blocks[i].id == id){
            return &blocks[i];
        }
    }
    return &undefinedBlock;
}