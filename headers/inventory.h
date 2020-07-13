#ifndef INVENTORY_H_
#define INVENTORY_H_

typedef struct item_cell{
	ItemComponent item;
	int qty;
    bool occupied;
}itmcell_t;

extern enum INV_PARAMS {INV_WIDTH = 4, INV_HEIGHT = 4};
extern itmcell_t invArray[INV_HEIGHT * INV_WIDTH];

extern ItemComponent undefinedItem;
extern ItemComponent *itemData;
extern int numItems;

extern bool showInv;
extern int selectedHotbar;//Number representation of currently selected hotbar slot

//Initializing item arrays
int ReadItemData();
int INV_InitRecipes();

//Basic inventory functions
void INV_Init();
void INV_DrawInv();
int INV_WriteCell(char *mode, int cell, int itemQty, ItemComponent item);

//Find functions
ItemComponent *find_item(char *name);
int INV_FindItem(ItemComponent itemNum);
int INV_FindEmpty(ItemComponent item);
//Lua functions
int register_item(lua_State *L);


#endif