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

ItemObject *NewItem(char *name, TilesheetObject tilesheet, int tileIndex){
	// Expand 'item_stack' to insert new item
    item_stack = realloc(item_stack, sizeof(ItemObject) * (num_items + 1));

	// Allocate space for item name
    item_stack[num_items].name = malloc(sizeof(char) * strlen(name));

	// Copy data into newly created item
    item_stack[num_items] = (ItemObject){name, nextID, tilesheet, tileIndex, false};
    DebugLog(D_ACT, "Created item id '%d' with name '%s'", nextID, name);

	// Increment stack counters
    num_items++;
    nextID++;
    return &item_stack[num_items - 1];
}

ItemObject *FindItem(char *name){
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