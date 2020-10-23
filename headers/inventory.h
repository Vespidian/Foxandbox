#ifndef INVENTORY_H_
#define INVENTORY_H_

typedef struct item_cell{
	ItemComponent *item;
	int qty;
    bool occupied;
}itmcell_t;

enum INV_PARAMS {INV_WIDTH = 8, INV_HEIGHT = 4};
extern enum INV_PARAMS inv_params;
extern itmcell_t invArray[INV_HEIGHT * INV_WIDTH];
extern itmcell_t mouseInv;

extern bool showInv;	
extern int selectedHotbar;//Number representation of currently selected hotbar slot

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
 *  \param qty The amount of specified item to write
 * 	\param item The item to write
 */
int INV_WriteCell(char *mode, int cell, int qty, ItemComponent *item);

/**
 *  Add qty of item to inventory array
 */
int INV_Add(int qty, ItemComponent *item, int cell);

/**
 *  Subtract qty of item from inventory array
 */
int INV_Subtract(int qty, ItemComponent *item, int cell);

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
#endif