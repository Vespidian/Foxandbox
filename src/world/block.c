#include "../global.h"
#include "../debug.h"
#include "block.h"

BlockObject air_block;

BlockObject undefined_block;

BlockObject *block_stack;

// Number of blocks in the 'block_stack'
unsigned int num_blocks = 0;

// ID to be used by next block created
static unsigned int nextID = 0;

void InitBlocks(){
	air_block = *NewBlock("air", NULL, NULL, &builtin_tilesheet, 1, false, false);
    // undefined_block = (BlockObject){-1, undefined_item, undefined_item, undefined_tilesheet, 0, false, false};
    undefined_block = *NewBlock("undefined", &undefined_item, &undefined_item, &undefined_tilesheet, 0, false, false);
    DebugLog(D_ACT, "Initialized block subsystem");
}

BlockObject *NewBlock(const char *name, ItemObject *item, ItemObject *break_item, TilesheetObject *tilesheet, int tile_index, bool allow_rotation, bool allow_autotile){
	// Expand the 'block_stack' to insert the new block
	block_stack = realloc(block_stack, sizeof(BlockObject) * (num_blocks + 1));
	
	// If the base item is not specified we create one
	if(item == NULL){
		// Check if item name already exits, if so use that item
		if((item = FindItem(name))->id == undefined_item.id){
			item = NewItem(name, tilesheet, tile_index);
		}
	}

	// If the 'break_item' is not specified we use the base item
    if(break_item == NULL){
        break_item = item;
    }

	// Copy data into new block
	block_stack[num_blocks] = (BlockObject){nextID, *item, *break_item, *tilesheet, tile_index, allow_rotation, allow_autotile};
    DebugLog(D_ACT, "Created block id '%d' with item name '%s'", num_blocks, item->name);

    nextID++;
	return &block_stack[num_blocks++];
}

// BlockObject *NewBlock(const char *name, ItemObject *item, ItemObject *break_item, TilesheetObject *tilesheet, int tile_index, bool allow_rotation, bool allow_autotile){
// 	// If the base item is not specified we create one
// 	if(item == NULL){
// 		// Check if item name already exits, if so use that item
// 		if((item = FindItem(name))->id == undefined_item.id){
// 			item = NewItem(name, tilesheet, tile_index);
// 		}
// 	}

// 	// If the 'break_item' is not specified we use the base item
//     if(break_item == NULL){
//         break_item = item;
//     }

// 	// Copy data into new block
// 	BlockObject *block = &(BlockObject){nextID, *item, *break_item, *tilesheet, tile_index, allow_rotation, allow_autotile};
//     DebugLog(D_ACT, "Created block id '%d' with item name '%s'", num_blocks, item->name);

//     nextID++;
// 	return block;
// }

// BlockObject *NewBlock_stack(const char *name, ItemObject *item, ItemObject *break_item, TilesheetObject *tilesheet, int tile_index, bool allow_rotation, bool allow_autotile){
// 	// Expand the 'block_stack' to insert the new block
// 	block_stack = realloc(block_stack, sizeof(BlockObject) * (num_blocks + 1));

// 	// Create and assign new block
//     block_stack[num_blocks] = *NewBlock(name, item, break_item, tilesheet, tile_index, allow_rotation, allow_autotile);

// 	return &block_stack[num_blocks++];
// }

BlockObject *FindBlock(const char *name){
    for(int i = 0; i < num_blocks; i++){
        if(strcmp(block_stack[i].item.name, name) == 0){
            return &block_stack[i];
        }
    }
    return &undefined_block;
}

BlockObject *FindBlock_id(unsigned int id){
    for(int i = 0; i < num_blocks; i++){
        if(block_stack[i].id == id){
            return &block_stack[i];
        }
    }
    return &undefined_block;
}