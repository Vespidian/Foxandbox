#include "../global.h"
#include "block.h"

BlockObject *blocks;
unsigned int num_blocks = 0;
static unsigned int nextID = 0;

BlockObject undefined_block;

void InitBlocks(){
    undefined_block = (BlockObject){-1, undefined_item.id, undefined_item.id, undefined_tilesheet.id, 0, false, false};
    DebugLog(D_ACT, "Initialized block subsystem");
}

void NewBlock(ItemObject *item, ItemObject *breakItem, TilesheetObject *tilesheet, int tileIndex, bool allow_rotation){
    blocks = realloc(blocks, sizeof(BlockObject) * (num_blocks + 1));
    if(breakItem == NULL){
        breakItem = item;
    }
    blocks[num_blocks] = (BlockObject){nextID, item->id, breakItem == NULL ? -1 : breakItem->id, tilesheet->id, tileIndex, allow_rotation, false};
    DebugLog(D_ACT, "Created block id '%d' with item name '%s'", num_blocks, item->name);
    num_blocks++;
    nextID++;
}

BlockObject *FindBlock(char *name){
    for(int i = 0; i < num_blocks; i++){
        if(strcmp(IDFindItem(blocks[i].item)->name, name) == 0){
            return &blocks[i];
        }
    }
    return &undefined_block;
}

BlockObject *IDFindBlock(unsigned int id){
    for(int i = 0; i < num_blocks; i++){
        if(blocks[i].id == id){
            return &blocks[i];
        }
    }
    return &undefined_block;
}