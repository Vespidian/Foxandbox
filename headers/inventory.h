#ifndef INVENTORY_H_
#define INVENTORY_H_

typedef struct item_cell{
	ItemComponent *item;
	int qty;
    bool occupied;
}itmcell_t;

extern enum INV_PARAMS {INV_WIDTH = 8, INV_HEIGHT = 4};
extern itmcell_t invArray[INV_HEIGHT * INV_WIDTH];

//Null item to set when item is missing members
extern ItemComponent undefinedItem;

//Dynamic array of all items
extern ItemComponent *itemData;

//Number of items used when interacting with itemData
extern int numItems;

extern bool showInv;	
extern int selectedHotbar;//Number representation of currently selected hotbar slot

/**
 *  \brief Iterate through all items to find specified name
 * 	\return A pointer to the found item or to undefinedItem if it cannot be found
 */
ItemComponent *find_item(char *name);

/**
 *  Initialize inventory array
 */
void INV_Init();

/**
 *  Render inventory and held items
 */
void INV_DrawInv();

/**
 *  \brief All purpose function for interacting with inventory
 *  \param mode The interaction method (sub, add, set)
 * 	\param cell The inventory cell to interact with
 *  \param itemQty The amount of specified item to write
 * 	\param item The item to write
 */
int INV_WriteCell(char *mode, int cell, int itemQty, ItemComponent *item);

/**
 *  Add qty of item to inventory array
 */
int INV_Add(int qty, ItemComponent *item);

/**
 *  Subtract qty of item from inventory array
 */
int INV_Subtract(int qty, ItemComponent *item);

/**
 *  Find the first slot with specified item
 */
int INV_FindItem(ItemComponent *item);

/**
 *  Find the first slot that contains specified item and is not full
 */
int INV_FindItem_NotFull(ItemComponent *item);

/**
 *  Find first slot that is not occupied
 */
int INV_FindEmpty();



//Lua functions
int register_item(lua_State *L);
int inventory_add(lua_State *L);


#endif