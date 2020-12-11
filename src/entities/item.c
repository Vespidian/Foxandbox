#include "../global.h"
#include "item.h"

ItemObject *items;
unsigned int numItems = 0;
static unsigned int nextID = 0;

ItemObject undefinedItem;

void InitItems(){
    undefinedItem = (ItemObject){"undefined", -1, undefinedTilesheet.id, 0, false};
    DebugLog(D_ACT, "Initialized item subsystem");
}

ItemObject *NewItem(char *name, TilesheetObject *tilesheet, int tileIndex){
    items = realloc(items, sizeof(ItemObject) * (numItems + 1));
    items[numItems].name = malloc(sizeof(char) * strlen(name));
    items[numItems] = (ItemObject){name, nextID, tilesheet->id, tileIndex, false};
    DebugLog(D_ACT, "Created item id '%d' with name '%s'", nextID, name);
    numItems++;
    nextID++;
    return &items[numItems - 1];
}

ItemObject *FindItem(char *name){
    for(int i = 0; i < numItems; i++){
        if(strcmp(items[i].name, name) == 0){
            return &items[i];
        }
    }
    return &undefinedItem;
}

ItemObject *IDFindItem(unsigned int id){
    for(int i = 0; i < numItems; i++){
        if(items[i].id == id){
            return &items[i];
        }
    }
    return &undefinedItem;
}