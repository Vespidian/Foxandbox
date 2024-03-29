#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <luaconf.h>

#include "headers/DataTypes.h"
#include "headers/ECS.h"

#include "headers/initialize.h"
#include "headers/render_systems.h"
#include "headers/data.h"
#include "headers/level_systems.h"
#include "headers/inventory.h"
#include "headers/lua_systems.h"
#include "headers/entity_systems.h"

enum INV_SECTIONS {ITM_SECT, QTY_SECT};
itmcell_t invArray[INV_HEIGHT * INV_WIDTH];

int selectedHotbar = 0;

const int INV_spacing = 14;
const int maxStack = 99;

Vector2 numOffset = {-2, 16};
itmcell_t mouseInv;
bool showInv = false;

RecipeComponent recipes[64];
int numberOfRecipes = 0;

void INV_Init(){
	int invPos = 0;
	for(int y = 0; y < INV_HEIGHT; y++){
		for(int x = 0; x < INV_WIDTH; x++){
			invPos++;
			invArray[invPos].occupied = false;
			invArray[invPos].qty = 0;
		}
	}
	DebugLog(D_ACT, "Inventory initialized");
}

int INV_InitRecipes(){
	FILE *file = fopen("data/recipes.dat", "r");
	if(file == NULL){
		return 1;
	}
	char buffer[512];
	int i = 0;
	char inItemTMP[64];
	char outItemTMP[64];
	
	while(fgets(buffer, sizeof(buffer), file)){
		//order of strtoks
		//, : , \n
		recipes[i].inQty = strtol(strtok(buffer, ","), NULL, 10);//Get number of inputed items
		strcpy(inItemTMP, strtok(NULL, ":"));//Get inputed item name
		recipes[i].outQty = strtol(strtok(NULL, ","), NULL, 10);//Get number of outputed items
		strcpy(outItemTMP, strtok(NULL, "\n"));//Get outputed item name
		
		bool inFound = false;
		bool outFound = false;
		for(int z = 0; z < 64; z++){//Find numerical value of 
			if(strcmp(inItemTMP, itemData[z].name) == 0 && !inFound){
				recipes[i].inItem = z;
				inFound = true;
			}
			if(strcmp(outItemTMP, itemData[z].name) == 0 && !outFound){
				recipes[i].outItem = z;
				outFound = true;
			}
			if(inFound == true && outFound == true){//If both values have been found end the loop
				break;
			}
		}
		i++;
	}
	numberOfRecipes = i;
	// printf("%d %s -> %d %s\n", recipes[0].inQty, itemData[recipes[0].inItem].name, recipes[0].outQty, itemData[recipes[0].outItem].name);
	// printf("%d %s -> %d %s", recipes[1].inQty, itemData[recipes[1].inItem].name, recipes[1].outQty, itemData[recipes[1].outItem].name);
	fclose(file);
	return 0;
}

void UpdateHotbar(){
	//Drawing the hotbar
	SDL_Rect hotBar = {WIDTH / 2 - (INV_WIDTH * 32) / 2 - ((INV_WIDTH + 1) * INV_spacing) / 2, HEIGHT - INV_spacing * 2 - 32, // ->
	INV_WIDTH * 32 + (INV_WIDTH + 1) * INV_spacing, INV_spacing * 2 + 32};
	AddToRenderQueue(renderer, find_tilesheet("ui"), 0, hotBar, -1, RNDRLYR_UI - 1);//Render hotbar background
	
	SDL_Rect slotRect = {0, HEIGHT - INV_spacing - 32, 32, 32};//Rect representing each slot in the hotbar
	for(int i = 0; i < INV_WIDTH; i++){//Iterate through the slots of the hotbar
		slotRect.x = (hotBar.x + 32 * i) + INV_spacing * (i + 1);
		if(selectedHotbar == i){
			SDL_Rect tmpRect = {-2, -2, 4, 4};
			tmpRect.x += slotRect.x;
			tmpRect.y += slotRect.y;
			tmpRect.w += slotRect.w;
			tmpRect.h += slotRect.h;
			AddToRenderQueue(renderer, find_tilesheet("ui"), 1, tmpRect, -1, RNDRLYR_UI - 1);
		}
		AddToRenderQueue(renderer, find_tilesheet("ui"), 8, slotRect, -1, RNDRLYR_UI);
		if(invArray[i].occupied == true && invArray[i].qty > 0){
			AddToRenderQueue(renderer, invArray[i].item->sheet, invArray[i].item->tile, slotRect, -1, RNDRLYR_INV_ITEMS);
			
			char itemqty[16];
			itoa(invArray[i].qty, itemqty, 10);
			RenderText_d(renderer, itemqty, slotRect.x + numOffset.x, slotRect.y + numOffset.y);
		}
	}
}

int hoveredCell = 0;
void INV_DrawInv(){
	UpdateHotbar();
	
	
	//Drawing the main inventory
	if(showInv){
		const int itemRectSize = 32;
		SDL_Rect invItemRect = {0, 0, itemRectSize, itemRectSize};//Position of the current item slot
		uiInteractMode = true;
		SDL_Rect invRect = {WIDTH / 2 - (INV_WIDTH * 32) / 2 - ((INV_WIDTH + 1) * INV_spacing) / 2, HEIGHT - (INV_HEIGHT + 2) * 32 - (INV_HEIGHT + 2) * INV_spacing, // ->
		INV_WIDTH * 32 + (INV_WIDTH + 1) * INV_spacing, INV_HEIGHT * 32 + (INV_HEIGHT + 1) * INV_spacing};
		
		AddToRenderQueue(renderer, find_tilesheet("ui"), 0, invRect, -1, RNDRLYR_UI - 1);//Render background of inventory
		
		SDL_Point mousePoint = {mouseTransform.screenPos.x, mouseTransform.screenPos.y};
		mousePoint.x = (mousePoint.x - invRect.x - INV_spacing / 2) / (itemRectSize + INV_spacing);
		mousePoint.y = (mousePoint.y - invRect.y - INV_spacing / 2) / (itemRectSize + INV_spacing);
		if(mousePoint.x + 1 <= INV_WIDTH && mousePoint.y + 1 <= INV_HEIGHT){
			hoveredCell = mousePoint.x % INV_WIDTH + (mousePoint.y * INV_WIDTH);
		}
		if(!gamePaused){
			//Check if the mouse is over the inventory
			if(SDL_PointInRect((SDL_Point *) &mouseTransform.screenPos, &invRect)){
				if(mouseClicked && hoveredCell >= 0 && hoveredCell < INV_WIDTH * INV_HEIGHT){
					if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT){//LEFT CLICK
						if(e.button.clicks == 2 && invArray[hoveredCell].occupied == true && &invArray[hoveredCell].item->name == &mouseInv.item->name){//Check for double clicked
							//Collect all the items of that type to the mouse pointer
							int totalFound = mouseInv.qty;
							while(INV_FindItem(mouseInv.item) != -1 && totalFound < maxStack){
								//Check if found stack can fit in mouse inventory
								if(totalFound + invArray[INV_FindItem(mouseInv.item)].qty < maxStack){
									totalFound += invArray[INV_FindItem(mouseInv.item)].qty;
									invArray[INV_FindItem(mouseInv.item)].qty = 0;
									invArray[INV_FindItem(mouseInv.item)].occupied = false;
								}else{//If it cannot, subtract from stack and fill mouse inventory
									invArray[INV_FindItem(mouseInv.item)].qty -= (maxStack - totalFound);
									totalFound = maxStack;
								}
							}
							mouseInv.occupied = true;
							mouseInv.qty = totalFound;
						}else{//Not a double click
							if(&invArray[hoveredCell].item->name == &mouseInv.item->name && mouseInv.occupied == true){//Check if the slot has the same item as the mouseInv
								int mouseRemainder = INV_WriteCell("add", hoveredCell, mouseInv.qty, mouseInv.item);
								if(mouseRemainder == 0){
									mouseInv.occupied = false;
									mouseInv.qty = 0;
								}else{
									mouseInv.occupied = true;
									mouseInv.qty = mouseRemainder;
								}
							}else{//Swap clicked item with held item
								itmcell_t tempInv = {mouseInv.item, mouseInv.qty, mouseInv.occupied};
								mouseInv.item = invArray[hoveredCell].item;
								mouseInv.qty = invArray[hoveredCell].qty;
								mouseInv.occupied = invArray[hoveredCell].occupied;
								invArray[hoveredCell].item = tempInv.item;
								invArray[hoveredCell].qty = tempInv.qty;
								invArray[hoveredCell].occupied = tempInv.occupied;
							}
						}
					}else if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT){//RIGHT CLICK
						if(mouseInv.qty > 0){//Check if mouse has any item
							if((invArray[hoveredCell].occupied == false || &invArray[hoveredCell].item->name == &mouseInv.item->name) && invArray[hoveredCell].qty < maxStack){//Check if cell is empty or has the same item as mouse
								if(mouseInv.qty > 1){//If there are multiple items in mouseInv add one to inv cell
									INV_WriteCell("add", hoveredCell, 1, mouseInv.item);
									mouseInv.qty--;
								}else{//If there was only 1, empty mouse slot
									INV_WriteCell("add", hoveredCell, 1, mouseInv.item);
									mouseInv.occupied = false;
									mouseInv.qty = 0;
								}
							}
						}else if(invArray[hoveredCell].qty > 1){//Otherwise take half the items in that inventory cell
							mouseInv.item = invArray[hoveredCell].item;
							mouseInv.qty = invArray[hoveredCell].qty / 2;
							mouseInv.occupied = true;
							INV_WriteCell("sub", hoveredCell, invArray[hoveredCell].qty / 2, invArray[hoveredCell].item);
						}
					}
				}
				if(invArray[hoveredCell].occupied == true){//If cell occupied and cursor is hovered, render item name
					SDL_Rect itemNameDisp = {invRect.x, invRect.y - itemRectSize, (strlen(invArray[hoveredCell].item->name) * 10) + 11, 28};
					AddToRenderQueue(renderer, find_tilesheet("ui"), 0, itemNameDisp, -1, RNDRLYR_UI - 1);//Background of item name dialogue
					
					char *tempChar = calloc(strlen(invArray[hoveredCell].item->name), sizeof(char));
					strcpy(tempChar, invArray[hoveredCell].item->name);
					tempChar[0] = toupper(tempChar[0]);

					RenderText_d(renderer, tempChar, itemNameDisp.x + 4, itemNameDisp.y + 6);//Item name
				}
			}else{//Otherwise drop item
				if(mouseClicked && activeLevel->collision[mouseTransform.tilePos.y][mouseTransform.tilePos.x] != 0){
					if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT){//LEFT CLICK
						if(mouseInv.occupied){
							Vector2 dropLocation = {mouseTransform.screenPos.x + mapOffsetPos.x - 16, mouseTransform.screenPos.y + mapOffsetPos.y - 16};
							
							DropItem(find_item(mouseInv.item->name), mouseInv.qty, dropLocation);
							mouseInv.qty = 0;
							mouseInv.item = &undefinedItem;
							mouseInv.occupied = false;
						}
					}else if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT){//RIGHT CLICK
						if(mouseInv.occupied){
							Vector2 dropLocation = {mouseTransform.screenPos.x + mapOffsetPos.x - 16, mouseTransform.screenPos.y + mapOffsetPos.y - 16};
							DropItem(find_item(mouseInv.item->name), 1, dropLocation);
							if(mouseInv.qty > 1){
								mouseInv.qty--;
							}else{
								mouseInv.qty = 0;
								mouseInv.item = &undefinedItem;
								mouseInv.occupied = false;
							}
						}
					}
				}
			}
		}
		
		for(int i = 0; i < INV_WIDTH * INV_HEIGHT; i++){//Inventory render loop
			int x = (i % INV_WIDTH), y = (i / INV_WIDTH);
			invItemRect.x = (invRect.x + itemRectSize * x) + INV_spacing * (x + 1);
			invItemRect.y = (invRect.y + itemRectSize * y) + INV_spacing * (y + 1);
			AddToRenderQueue(renderer, find_tilesheet("ui"), 8, invItemRect, -1, RNDRLYR_UI);//Draw the background of each cell
			if(invArray[i].occupied == true && invArray[i].qty > 0){//Check if item exists in cell and render it
				if(i == hoveredCell && SDL_PointInRect((SDL_Point *) &mouseTransform.screenPos, &invRect) && !gamePaused){
					SDL_Rect largerRect = {invItemRect.x - 8, invItemRect.y - 8, invItemRect.w + 16, invItemRect.h + 16};
					AddToRenderQueue(renderer, invArray[i].item->sheet, invArray[i].item->tile, largerRect, -1, RNDRLYR_INV_ITEMS);
				}else{
					AddToRenderQueue(renderer, invArray[i].item->sheet, invArray[i].item->tile, invItemRect, -1, RNDRLYR_INV_ITEMS);
				}
				
				char itemqty[16];
				itoa(invArray[i].qty, itemqty, 10);
				RenderText_d(renderer, itemqty, invItemRect.x + numOffset.x, invItemRect.y + numOffset.y);//Item amount
			}else{
				invArray[i].occupied = false;
				invArray[i].qty = 0;
			}
		}
		//Drawing the mouse inventory
		if(mouseInv.occupied == true && mouseInv.qty > 0){
			SDL_Rect mouseItem = {mouseTransform.screenPos.x - 16, mouseTransform.screenPos.y - 16, itemRectSize, itemRectSize};
			AddToRenderQueue(renderer, mouseInv.item->sheet, mouseInv.item->tile, mouseItem, 255, RNDRLYR_INV_ITEMS);
			
			char itemqty[16];
			itoa(mouseInv.qty, itemqty, 10);
			RenderText_d(renderer, itemqty, mouseItem.x + numOffset.x, mouseItem.y + numOffset.y);//Draw item quantity for each item
		}
	}else{
		uiInteractMode = false;
	}
}


int INV_WriteCell(char *mode, int cell, int qty, ItemComponent *item){
	if(cell > INV_HEIGHT * INV_WIDTH){
		printf("Error: Item location out of bounds!\n");
		return 1;
	}

	if(qty > 0){
		
		if(strcmp(mode, "set") == 0){
			invArray[cell].item = item;
			invArray[cell].occupied = true;
			if(qty < maxStack){
				invArray[cell].qty = qty;
				invArray[cell].occupied = true;
			}else{
				invArray[cell].qty = maxStack;
				invArray[cell].occupied = true;
			}
		}else if(strcmp(mode, "add") == 0){
			return INV_Add(qty, item, cell);
		}else if(strcmp(mode, "sub") == 0){
			return INV_Subtract(qty, item, cell);
		}
	}
	return 0;
}

int INV_Add(int qty, ItemComponent *item, int cell){
	if(qty > maxStack){
		qty = maxStack;
	}
	if(item != find_item("air")){//Make sure item is not air
		if(cell > -1 && cell < INV_WIDTH * INV_HEIGHT){
			if((invArray[cell].item == item && invArray[cell].occupied) || !invArray[cell].occupied){
					printf("here\n");
				int remainder = 0;
				if(!invArray[cell].occupied){
					invArray[cell].occupied = true;
					invArray[cell].item = item;
					invArray[cell].qty = qty;
				}else{
					if(invArray[cell].qty + qty > maxStack){
						remainder = qty - (maxStack - invArray[cell].qty);
						invArray[cell].qty = maxStack;
					}else{
						invArray[cell].qty += qty;
					}
				}
				printf("%d\n", remainder);
				return remainder;
			}
		}

		if(INV_FindItem_NotFull(item) != -1 && invArray[INV_FindItem_NotFull(item)].qty < maxStack){//Check if item exists in inventory and can fit more items
			if(invArray[INV_FindItem_NotFull(item)].qty + qty <= maxStack){//Check if the qty can fit in the stack
				invArray[INV_FindItem_NotFull(item)].qty += qty;
			}else{//If it cant add to the next empty slot as well
				qty -= maxStack - invArray[INV_FindItem_NotFull(item)].qty;
				invArray[INV_FindItem_NotFull(item)].qty = maxStack;
				int emptySlot = INV_FindEmpty();
				invArray[emptySlot].item = item;
				invArray[emptySlot].qty = qty;
				invArray[emptySlot].occupied = true;
			}
			return 0;
		}else{//If it does not exist or is full create it in a new slot
			invArray[INV_FindEmpty()].item = item;
			invArray[INV_FindEmpty()].qty = qty;
			invArray[INV_FindEmpty()].occupied = true;
		}
	}
	return 0;
}

int INV_Subtract(int qty, ItemComponent *item, int cell){
	if(cell > -1 && cell < INV_WIDTH * INV_HEIGHT){
		if(invArray[cell].qty <= qty){
			invArray[cell].occupied = false;
		}else{
			invArray[cell].qty -= qty;
		}
		return 0;
	}
	while(INV_FindItem(item) != -1){//Loop until there are no more of specified item
		if(invArray[INV_FindItem(item)].qty >= qty){
			if(invArray[INV_FindItem(item)].qty == qty){
				invArray[INV_FindItem(item)].occupied = false;
			}else{
				invArray[INV_FindItem(item)].qty -= qty;
			}
			return 0;
		}
		if(invArray[INV_FindItem(item)].qty < qty){
			qty -= invArray[INV_FindItem(item)].qty;
			invArray[INV_FindItem(item)].occupied = false;
		}
	}
	return qty;
}

int INV_FindItem(ItemComponent *item){//Find the first slot with specified item
	for(int i = 0; i < INV_WIDTH * INV_HEIGHT; i++){//Loop through inventory
		if(&invArray[i].item->name == &item->name && invArray[i].occupied == true){
			return i;
		}
	}
	return -1;//Item does not exist in inventory
}

int INV_FindItem_NotFull(ItemComponent *item){//Find the first slot that has that item and is not full
	for(int i = 0; i < INV_WIDTH * INV_HEIGHT; i++){//Loop through inventory
		if(&invArray[i].item->name == &item->name && invArray[i].occupied == true && invArray[i].qty < maxStack){
			return i;
		}
	}
	return -1;
}

int INV_FindEmpty(){//Find first slot that is not occupied
	for(int i = 0; i < INV_WIDTH * INV_HEIGHT; i++){//Loop through inventory
		if(invArray[i].occupied == false){//If there is not an item there return that slot 
			return i;
		}
	}
	return -1;//Every slot in the inventory is filled up / Item cannot fit in inventory
}