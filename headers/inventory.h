#ifndef INVENTORY_H_
#define INVENTORY_H_

// extern const int INV_WIDTH;
// extern const int INV_HEIGHT;
extern enum INV_PARAMS {INV_WIDTH = 4, INV_HEIGHT = 4};

extern INV_Item itemData[64];

extern bool showInv;

extern int selectedHotbar;

void INV_Init();
void INV_DrawInv();
int INV_WriteCell(char *mode, int cell, int itemQty, int itemNum);
int INV_ReadCell(char *mode, int cell);
int INV_FindEmpty(int type);


int ReadItemData();

#endif