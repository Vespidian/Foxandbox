#include "../global.h"
#include "block.h"

BlockObject *blocks;
unsigned int numBlocks = 0;

BlockObject undefinedBlock;

void InitBlocks(){
    undefinedBlock = (BlockObject){&undefinedItem, &undefinedItem, &undefinedTilesheet, 0, false};
    DebugLog(D_ACT, "Initialized block subsystem");
}

void CreateBlock(ItemObject *item, ItemObject *breakItem, TilesheetObject *tilesheet, int tileIndex, bool allowRotation){
    blocks = realloc(blocks, sizeof(BlockObject) * (numBlocks + 1));
    if(breakItem == NULL){
        breakItem = item;
    }
    blocks[numBlocks] = (BlockObject){item, breakItem, tilesheet, tileIndex, allowRotation};
    DebugLog(D_ACT, "Created block id '%d' with item name '%s'", numBlocks, item->name);
    numBlocks++;
}

BlockObject *FindBlock(char *name){
    for(int i = 0; i < numBlocks; i++){
        if(strcmp(blocks[i].item->name, name) == 0){
            return &blocks[i];
        }
    }
    return &undefinedBlock;
}