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
#include "headers/renderSystems.h"
#include "headers/data.h"
#include "headers/tileMap.h"
#include "headers/inventory.h"

enum INV_SECTIONS {ITM_SECT, QTY_SECT};
itmcell_t invArray[INV_HEIGHT * INV_WIDTH];

int selectedHotbar = 0;

const int INV_spacing = 8;
const int maxStack = 99;

Vector2 numOffset = {-2, 16};
itmcell_t mouseInv;
bool showInv = false;

ItemComponent undefinedItem;
ItemComponent *itemData;
int numItems = -1;


RecipeComponent recipes[64];
int numberOfRecipes = 0;

ItemComponent *find_item(char *name){
	for(int i = 0; i < numItems + 1; i++){
		if(strcmp(name, itemData[i].name) == 0){
			return &itemData[i];
		}
	}
	return &undefinedItem;
}

void INV_Init(){
	int invPos = 0;
	for(int y = 0; y < INV_HEIGHT; y++){
		for(int x = 0; x < INV_WIDTH; x++){
			invPos++;
			invArray[invPos].occupied = false;
			invArray[invPos].qty = 0;
		}
	}
	// printf("%s\n", find_item("stone")->name);
	// printf("%s\n", itemData[0].name);
	// INV_WriteCell("set", 0, 2, undefinedItem);
	// INV_WriteCell("set", 0, 2, *find_item("stone"));
	// INV_WriteCell("set", 12, 90, 2);
	// INV_WriteCell("set", 8, 1, 3);
	// INV_WriteCell("set", 3, 1, 1);
	// INV_WriteCell("set", 4, 1, 0);
	// INV_WriteCell("set", 11, 0, -1);
	// INV_WriteCell("set", 14, 20, 4);
}


int register_item(lua_State *L){
	numItems++;
	itemData = realloc(itemData, (numItems + 10) * sizeof(ItemComponent));

	luaL_checktype(L, 1, LUA_TTABLE);

	lua_getfield(L, -1, "name");
	if(lua_tostring(L, -1) != NULL && strlen(lua_tostring(L, -1)) > 0){
		itemData[numItems].name = malloc(sizeof(char) * (strlen(lua_tostring(L, -1)) + 1));
		strcpy(itemData[numItems].name, lua_tostring(L, -1));
	}else{
		itemData[numItems].name = malloc(sizeof(char) * (strlen("undefined") + 1));
		strcpy(itemData[numItems].name, "undefined");
	}

	lua_getfield(L, -2, "sheet");
	if(lua_tostring(L, -1) != NULL){
		itemData[numItems].sheet = *find_tilesheet((char *)lua_tostring(L, -1));
	}else{
		itemData[numItems].sheet = undefinedSheet;
	}

	lua_getfield(L, -3, "tile_index");
	if(lua_tonumber(L, -1)){
		itemData[numItems].tile = lua_tonumber(L, -1);
	}else{
		itemData[numItems].tile = -1;
	}

	itemData[numItems].isBlock = false;
	return 0;
}

int inventory_add(lua_State *L){
	char *name;
	int qty = 0;
	if(lua_tostring(L, 1) != NULL){
		name = malloc(sizeof(char) * strlen(lua_tostring(L, 1)));
		strcpy(name, lua_tostring(L, 1));
	}
	if(lua_tonumber(L, 2) != NULL){
		qty = lua_tonumber(L, 2);
	}
	INV_Add(qty, find_item(name));
	free(name);
	return 0;
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
	SDL_Rect hotBar = {WIDTH / 2 - INV_WIDTH * 32 + (INV_WIDTH + 1) * INV_spacing, HEIGHT - INV_spacing * 2 - 32, // ->
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
			AddToRenderQueue(renderer, &invArray[i].item->sheet, invArray[i].item->tile, slotRect, -1, RNDRLYR_INV_ITEMS);
			
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
		SDL_Rect invRect = {WIDTH / 2 - INV_WIDTH * 32 + (INV_WIDTH + 1) * INV_spacing, HEIGHT - INV_HEIGHT * 32 + (INV_HEIGHT + 1) * INV_spacing - 132, // ->
		INV_WIDTH * 32 + (INV_WIDTH + 1) * INV_spacing, INV_HEIGHT * 32 + (INV_HEIGHT + 1) * INV_spacing};
		
		AddToRenderQueue(renderer, find_tilesheet("ui"), 0, invRect, -1, RNDRLYR_UI - 1);//Render background of inventory
		
		SDL_Point mousePoint = {mouseTransform.screenPos.x, mouseTransform.screenPos.y};
		mousePoint.x = (mousePoint.x - invRect.x - INV_spacing / 2) / (itemRectSize + INV_spacing);
		mousePoint.y = (mousePoint.y - invRect.y - INV_spacing / 2) / (itemRectSize + INV_spacing);
		if(mousePoint.x + 1 <= INV_WIDTH && mousePoint.y + 1 <= INV_HEIGHT){
			hoveredCell = mousePoint.x % INV_WIDTH + (mousePoint.y * INV_WIDTH);
		}
		if(SDL_PointInRect(&mouseTransform.screenPos, &invRect) && hoveredCell >= 0 && hoveredCell < INV_WIDTH * INV_HEIGHT){
			if(mouseClicked == true){
				if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT){//LEFT CLICK
					if(e.button.clicks == 2 && invArray[hoveredCell].occupied == true && &invArray[hoveredCell].item->name == &mouseInv.item->name){//Check for double clicked
						//Collect all the items of that type to the mouse pointer
						int totalFound = 0;
						while(INV_FindItem(mouseInv.item) != -1 && totalFound < maxStack){
							totalFound += invArray[INV_FindItem(mouseInv.item)].qty;
							invArray[INV_FindItem(mouseInv.item)].qty = 0;
							invArray[INV_FindItem(mouseInv.item)].occupied = false;
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
						if(invArray[hoveredCell].occupied == false || &invArray[hoveredCell].item->name == &mouseInv.item->name && invArray[hoveredCell].qty < maxStack){//Check if cell is empty or has the same item as mouse
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
						// printf("here\n");
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
		}else{
			if(mouseClicked == true && levels[0].collision[mouseTransform.tilePos.y][mouseTransform.tilePos.x] != 0){
				if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT){//LEFT CLICK
					if(mouseInv.occupied == true){
						Vector2 dropLocation = {mouseTransform.screenPos.x + mapOffsetPos.x - 16, mouseTransform.screenPos.y + mapOffsetPos.y - 16};
						DropItem(find_item(mouseInv.item->name), mouseInv.qty, dropLocation);
						mouseInv.qty = 0;
						mouseInv.item = &undefinedItem;
						mouseInv.occupied = false;
					}
				}else if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT){//RIGHT CLICK
					if(mouseInv.occupied == true){
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
		
		for(int i = 0; i < INV_WIDTH * INV_HEIGHT; i++){//Inventory render loop
			int x = (i % INV_WIDTH), y = (i / INV_WIDTH);
			invItemRect.x = (invRect.x + itemRectSize * x) + INV_spacing * (x + 1);
			invItemRect.y = (invRect.y + itemRectSize * y) + INV_spacing * (y + 1);
			AddToRenderQueue(renderer, find_tilesheet("ui"), 8, invItemRect, -1, RNDRLYR_UI);//Draw the background of each cell
			if(invArray[i].occupied == true && invArray[i].qty > 0){//Check if item exists in cell and render it
				AddToRenderQueue(renderer, &invArray[i].item->sheet, invArray[i].item->tile, invItemRect, -1, RNDRLYR_INV_ITEMS);
				
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
			AddToRenderQueue(renderer, &mouseInv.item->sheet, mouseInv.item->tile, mouseItem, 255, RNDRLYR_INV_ITEMS);
			
			char itemqty[16];
			itoa(mouseInv.qty, itemqty, 10);
			RenderText_d(renderer, itemqty, mouseItem.x + numOffset.x, mouseItem.y + numOffset.y);//Draw item quantity for each item
		}
	}else{
		uiInteractMode = false;
	}
}


int INV_WriteCell(char *mode, int cell, int itemQty, ItemComponent *item){
	if(cell > INV_HEIGHT * INV_WIDTH){
		printf("Error: Item location out of bounds!\n");
		return 1;
	}

	if(itemQty != NULL && itemQty != 0){
		
		if(strcmp(mode, "set") == 0){
			invArray[cell].item = item;
			invArray[cell].occupied = true;
			if(itemQty < maxStack){
				invArray[cell].qty = itemQty;
				invArray[cell].occupied = true;
			}else{
				invArray[cell].qty = maxStack;
				invArray[cell].occupied = true;
			}
		}else if(strcmp(mode, "add") == 0){
			invArray[cell].item = item;
			invArray[cell].occupied = true;
			if(strcmp(invArray[cell].item->name, item->name) == 0 && invArray[cell].qty <= maxStack){//Check if item is of different type or exceeding limit
				if(invArray[cell].qty + itemQty > maxStack){//Check if adding item will cause cell to exceed maxStack size
					int remainder = (itemQty - (maxStack - invArray[cell].qty));
					invArray[cell].qty += maxStack - invArray[cell].qty;//Fill the cell
					return remainder;//Return the remainder if the stack is full
				}else{
					invArray[cell].qty += itemQty;
					invArray[cell].occupied = true;
					return 0;
				}
			}
		}else if(strcmp(mode, "sub") == 0){
			if(strcmp(invArray[cell].item->name, item->name) == 0){
				if(invArray[cell].qty > 0){
					invArray[cell].qty -= itemQty;
					invArray[cell].occupied = true;
				}else{
					invArray[cell].qty = 0;
					invArray[cell].occupied = false;
				}
			}
		}
	}else{
		invArray[cell].qty = 0;
		invArray[cell].occupied = false;
	}
	return 0;
}

int INV_Add(int qty, ItemComponent *item){
	if(INV_FindEmpty(item) != -1 && item != find_item("air")){//Make sure inventory is not full and item is not air
		if(INV_FindItem(item) != -1 && invArray[INV_FindItem(item)].qty < maxStack){//Check if item exists and can fit more items
			if(invArray[INV_FindItem(item)].qty + qty <= maxStack){//Check if the qty can fit in the stack
				invArray[INV_FindItem(item)].qty += qty;
			}else{//If it cant add to the next empty slot as well
				qty -= maxStack - invArray[INV_FindItem(item)].qty;
				invArray[INV_FindItem(item)].qty = maxStack;
				invArray[INV_FindEmpty()].item = item;
				invArray[INV_FindEmpty()].qty = qty;
				invArray[INV_FindEmpty()].occupied = true;
			}
			return 0;
		}else{//If it does not exist create it
			invArray[INV_FindEmpty()].item = item;
			invArray[INV_FindEmpty()].qty = qty;
			invArray[INV_FindEmpty()].occupied = true;
		}
	}
}
int INV_Subtract(int qty, ItemComponent *item){
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

int INV_FindItem(ItemComponent *item){
	int itemQtyFound = 0;
	for(int i = 0; i < INV_WIDTH * INV_HEIGHT; i++){
		if(&invArray[i].item->name == &item->name && invArray[i].occupied == true){
			return i;
		}
	}
	return -1;//Item does not exist in inventory
}
int INV_FindEmpty(){
	for(int i = 0; i < INV_WIDTH * INV_HEIGHT; i++){
		if(invArray[i].occupied == false){
			return i;
		}
	}
	return -1;//Inventory full
}