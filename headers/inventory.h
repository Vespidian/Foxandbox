#ifndef INVENTORY_H_
#define INVENTORY_H_

// extern const int INV_WIDTH;
// extern const int INV_HEIGHT;
extern enum INV_PARAMS {INV_WIDTH = 4, INV_HEIGHT = 4};
extern int invArray[INV_HEIGHT * INV_WIDTH][2];

extern INV_ItemComponent itemData[64];//Dictionary of current items

extern bool showInv;
extern int selectedHotbar;//Number representation of currently selected hotbar slot

//Initializing item arrays
int ReadItemData();
int INV_InitRecipes();

//Basic inventory functions
void INV_Init();
void INV_DrawInv();
int INV_WriteCell(char *mode, int cell, int itemQty, int itemNum);

//Find functions
int INV_FindEmpty(int type);
int INV_FindItem(int itemNum);

#endif