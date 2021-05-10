#include "../global.h"
#include "item.h"

ItemObject *items;
unsigned int num_items = 0;
static unsigned int nextID = 0;

ItemObject undefined_item;

void InitItems(){
    undefined_item = (ItemObject){"undefined", -1, undefined_tilesheet.id, 0, false};
    DebugLog(D_ACT, "Initialized item subsystem");
}

ItemObject *NewItem(char *name, TilesheetObject *tilesheet, int tileIndex){
    items = realloc(items, sizeof(ItemObject) * (num_items + 1));
    items[num_items].name = malloc(sizeof(char) * strlen(name));
    items[num_items] = (ItemObject){name, nextID, tilesheet->id, tileIndex, false};
    DebugLog(D_ACT, "Created item id '%d' with name '%s'", nextID, name);
    num_items++;
    nextID++;
    return &items[num_items - 1];
}

ItemObject *FindItem(char *name){
    for(int i = 0; i < num_items; i++){
        if(strcmp(items[i].name, name) == 0){
            return &items[i];
        }
    }
    return &undefined_item;
}

ItemObject *IDFindItem(unsigned int id){
    for(int i = 0; i < num_items; i++){
        if(items[i].id == id){
            return &items[i];
        }
    }
    return &undefined_item;
}