#include "../global.h"
#include "item.h"

ItemObject undefined_item;

ItemObject *item_stack;

// Number of items in the item stack
unsigned int num_items = 0;

// ID to be used by the next item created
static unsigned int nextID = 0;

void InitItems(){
    undefined_item = (ItemObject){"undefined", -1, undefined_tilesheet, 0, false};
    DebugLog(D_ACT, "Initialized item subsystem");
}

ItemObject *NewItem(const char *name, TilesheetObject *tilesheet, int tile_index){
	// Check if item with same name already exists
	if(FindItem(name)->id != undefined_item.id){
		return FindItem(name);
	}

	// Expand 'item_stack' to insert new item
    item_stack = realloc(item_stack, sizeof(ItemObject) * (num_items + 1));

	// Allocate space for item name
    item_stack[num_items].name = malloc(sizeof(char) * (strlen(name) + 1));

	// Copy data into newly created item
    item_stack[num_items] = (ItemObject){name, nextID, *tilesheet, tile_index, false};
    DebugLog(D_ACT, "Created item id '%d' with name '%s'", nextID, name);

	// Increment stack counters and return newly created item
    nextID++;
    return &item_stack[num_items++];
}

ItemObject *FindItem(const char *name){
    for(int i = 0; i < num_items; i++){
        if(strcmp(item_stack[i].name, name) == 0){
            return &item_stack[i];
        }
    }
    return &undefined_item;
}

ItemObject *FindItem_id(unsigned int id){
    for(int i = 0; i < num_items; i++){
        if(item_stack[i].id == id){
            return &item_stack[i];
        }
    }
    return &undefined_item;
}