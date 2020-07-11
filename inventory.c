#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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

BlockComponent undefinedBlock;
BlockComponent *blockData;
int numBlocks = -1;

AutotileComponent *autotile;
int numAutotiles = -1;



RecipeComponent recipes[64];
int numberOfRecipes = 0;
int grabTime = 0;

ItemComponent *find_item(char *name){
	for(int i = 0; i < numItems; i++){
		if(strcmp(name, itemData[i].name) == 0){
			return &itemData[i];
		}
	}
	return &undefinedItem;
}

BlockComponent *find_block(char *name){
	for(int i = 0; i < numBlocks; i++){
		if(strcmp(name, blockData[i].item->name) == 0){
			return &blockData[i];
		}
	}
	return &undefinedBlock;
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
	itemData = realloc(itemData, (numItems + 1) * sizeof(ItemComponent));

	luaL_checktype(L, 1, LUA_TTABLE);

	lua_getfield(L, -1, "name");
	if(lua_tostring(L, -1) != NULL && strlen(lua_tostring(L, -1)) > 0){
		// itemData[numItems].name = malloc(sizeof(char *) * strlen(lua_tostring(L, -1)));
		strcpy(itemData[numItems].name, lua_tostring(L, -1));
		printf("%s\n", find_item("stone")->name);
	}else{
		strcpy(itemData[numItems].name, "undefined");
	}

	lua_getfield(L, -2, "sheet");
	if(lua_tostring(L, -1) != NULL){
		itemData[numItems].sheet = *find_tilesheet((char *)lua_tostring(L, -1));
	}else{
		itemData[numItems].sheet = undefinedSheet;
	}

	lua_getfield(L, -3, "tile_index");
	if(lua_tonumber(L, -1) != NULL){
		itemData[numItems].tile = lua_tonumber(L, -1);
	}else{
		itemData[numItems].tile = -1;
	}

	itemData[numItems].isBlock = false;
	return 0;
}

int register_block(lua_State *L){
	numBlocks++;
	blockData = realloc(blockData, (numBlocks + 1) * sizeof(BlockComponent));

	luaL_checktype(L, 1, LUA_TTABLE);

	lua_getfield(L, -1, "name");
	if(strcmp(find_item((char *)lua_tostring(L, -1))->name, "undefined") == 0){//Check if the item already exists
		numItems++;
		itemData = realloc(itemData, (numItems + 1) * sizeof(ItemComponent));

		if(lua_tostring(L, -1) != NULL && strlen(lua_tostring(L, -1)) > 0){
			strcpy(itemData[numItems].name, lua_tostring(L, -1));
		}else{
			strcpy(itemData[numItems].name, "undefined");
		}

		lua_getfield(L, -2, "item_sheet");
		if(lua_tostring(L, -1) != NULL){
			itemData[numItems].sheet = *find_tilesheet((char *)lua_tostring(L, -1));
		}else{
			itemData[numItems].sheet = undefinedSheet;
		}

		lua_getfield(L, -3, "item_tile_index");
		if(lua_tonumber(L, -1) != NULL){
			itemData[numItems].tile = lua_tonumber(L, -1);
		}else{
			itemData[numItems].tile = 0;
		}

		blockData[numBlocks].item = &itemData[numItems];
		itemData[numItems].isBlock = true;
	}else{//If it does, use it
		blockData[numBlocks].item = find_item((char *)lua_tostring(L, -1));
		find_item((char *)lua_tostring(L, -1))->isBlock = true;
	}

	lua_getfield(L, -4, "block_sheet");
	if(lua_tostring(L, -1) != NULL){
		blockData[numBlocks].sheet = *find_tilesheet((char *)lua_tostring(L, -1));
	}else{
		blockData[numBlocks].sheet = undefinedSheet;
	}

	lua_getfield(L, -5, "block_tile_index");
	if(lua_tonumber(L, -1) != NULL){
		blockData[numBlocks].tile = lua_tonumber(L, -1);
	}else{
		blockData[numBlocks].tile = 0;
	}


	lua_getfield(L, -6, "dropped_item");
	if(lua_tostring(L, -1) != NULL){
		if(find_item((char *)lua_tostring(L, -1)) != NULL){
			blockData[numBlocks].dropItem = find_item((char *)lua_tostring(L, -1));
		}else{
			blockData[numBlocks].dropItem = blockData[numBlocks].item;
		}
	}else{
		blockData[numBlocks].dropItem = &undefinedItem;
	}

	lua_getfield(L, -7, "dropped_qty");
	if(lua_tonumber(L, -1) != NULL){
		blockData[numBlocks].dropQty = lua_tonumber(L, -1);
	}else{
		blockData[numBlocks].dropQty = 1;
	}


	blockData[numBlocks].flags = malloc(sizeof(char **));
	lua_getfield(L, -8, "flags");
	if(lua_istable(L, -1)){
		size_t len = lua_rawlen(L, -1);
		int numFlags = 0;
		for(int i = 0; i <= len; i++){
			lua_pushinteger(L, i + 1);
			lua_gettable(L, -2);	
			if(lua_tostring(L, -1) != NULL){
				// char *temp = calloc(strlen(lua_tostring(L, -1)), sizeof(char));
				// strcpy(temp, lua_tostring(L, -1));
				blockData[numBlocks].flags[numFlags] = calloc(strlen(lua_tostring(L, -1)) + 1, sizeof(char));
				// printf("%d   ", strlen(lua_tostring(L, -1)));
				strcpy(blockData[numBlocks].flags[numFlags], lua_tostring(L, -1));
				// blockData[numBlocks].flags[numFlags][strlen(blockData[numBlocks].flags[numFlags])] = '\0';
				// strcpy(blockData[numBlocks].flags[numFlags], temp);

				printf("%s\n", blockData[numBlocks].flags[numFlags]);
				numFlags++;
			}
			lua_pop(L, 1);
		}
	}
	blockData[numBlocks].autoTile = false;

	return 0;
}	

int populate_autotile(lua_State *L){
	numAutotiles++;
	autotile = realloc(autotile, (numAutotiles + 1) * sizeof(AutotileComponent));

	luaL_checktype(L, 1, LUA_TTABLE);

	//Get name
	lua_getfield(L, -1, "name");
	if(lua_tostring(L, -1) != NULL && strlen(lua_tostring(L, -1)) > 0){
		autotile[numAutotiles].name = calloc(strlen(lua_tostring(L, -1)), sizeof(char));
		strcpy(autotile[numAutotiles].name, lua_tostring(L, -1));
	}else{//Undefined
		autotile[numAutotiles].name = calloc(strlen("undefined"), sizeof(char));
		strcpy(autotile[numAutotiles].name, "undefined");
	}


	//Get base item
	lua_getfield(L, -2, "base_block");
	if(lua_tostring(L, -1) != NULL && strlen(lua_tostring(L, -1)) > 0){
		autotile[numAutotiles].baseBlock = find_block((char *)lua_tostring(L, -1));
	}else{//Undefined
		autotile[numAutotiles].baseBlock = &undefinedBlock;
	}

	//Get sub item
	lua_getfield(L, -3, "sub_block");
	if(lua_tostring(L, -1) != NULL && strlen(lua_tostring(L, -1)) > 0){
		autotile[numAutotiles].subBlock = find_block((char *)lua_tostring(L, -1));
	}else{//Undefined
		autotile[numAutotiles].subBlock = &undefinedBlock;
	}

	//For loop to loop between the base and sub
	BlockComponent block;
	block.item = autotile[numAutotiles].baseBlock->item;
	block.dropItem = autotile[numAutotiles].baseBlock->dropItem;
	block.dropQty = autotile[numAutotiles].baseBlock->dropQty;
	block.sheet = autotile[numAutotiles].baseBlock->sheet;
	block.autoTile = true;
	for(int i = 1; i <= 46; i++){
		block.tile = i;
		autotile[numAutotiles].auto_block[i - 1] = (BlockComponent_local)block;
		snprintf(autotile[numAutotiles].auto_block[i - 1].item.name, strlen(autotile[numAutotiles].baseBlock->item->name) + 2,\
		"%s%d", autotile[numAutotiles].baseBlock->item->name, i);
		printf("%s\n", autotile[numAutotiles].auto_block[i - 1].item.name);
	}

	return 0;
}

int ReadItemData(){
	// FILE *file = fopen("data/items.dat", "r");
	// if(file == NULL){
	// 	return 1;
	// }
	// char buffer[512];
	// char declarationType[64];
	
	
	
	
	// int itemCounter = 0;
	// while(fgets(buffer, sizeof(buffer), file)){
	// 	if(buffer[0] != '\n'){
	// 		if(buffer[0] == '\t' && buffer[1] != '\n'){
	// 			strcpy(buffer, strshft_l(buffer, 1));
				
				// printf("%s	-> %d\n", declarationType, itemCounter);
				/*if(strcmp(declarationType, "CRAFTING_INGREDIENTS") == 0){
					itemData = realloc(itemData, (itemCounter + 1) * sizeof(ItemComponent *));
					strcpy(itemData[itemCounter].name, strtok(buffer, ":"));
					strcpy(itemData[itemCounter].description, strtok(NULL, "|"));
					
					printf("%s\n", itemData[itemCounter].name);
				}else if(strcmp(declarationType, "BLOCKS") == 0){//Check if iterating through blocks
					
					char tileNum[16];
					int flagCount = 1;
					// char **flags = malloc(flagCount * sizeof(char *));
					blockData[itemCounter].flags[flagCount] = malloc(flagCount * sizeof(char *));
					char flag[64];
					
					
					strcpy(blockData[itemCounter].item.name, strtok(buffer, ":"));
					strcpy(tileNum, strshft_l(strtok(NULL, "|"), 3));
					
					printf("%s:\n", blockData[itemCounter].item.name);
					strcpy(flag, strtok(NULL, "|"));
					while(strcmp(flag, "\n") != 0){
						
						// *flags = realloc(*flags, (flagCount + 1) * sizeof(char*));
						// flags[flagCount] = malloc(sizeof(char[strlen(flag)]));
						blockData[itemCounter].flags[flagCount] = malloc(sizeof(char[strlen(flag)]));
						strcpy(blockData[itemCounter].flags[flagCount], flag);
						
						printf("+   %s\n", blockData[flagCount].flags[flagCount]);
						strcpy(flag, strtok(NULL, "|"));
						flagCount++;
					}
				}*/
		// 		itemCounter++;
		// 		// printf("Current declarationType: %s\n", declarationType);
		// 	}else if(buffer[0] == ':' && buffer[1] == ':'){
		// 		strcpy(buffer, strshft_l(buffer, 2));
		// 		// strshft_l(buffer, 2);
		// 		itemCounter = 0;
		// 		buffer[strlen(buffer) - 1] = '\0';
		// 		strcpy(declarationType, buffer);
		// 	}/*else  if(buffer[0] == '/' && buffer[1] == '/'){
				
		// 	} */
		// }
		// printf("%s >> %s\n", itemData[itemCounter].name, itemData[itemCounter].description);
		// printf("%s >> %s\n", itemData[0].name, itemData[0].description);
	// }
	// printf("%s\n", itemData[0].name);
	// INV_InitRecipes();
	// fclose(file);
	// return 0;
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
	AddToRenderQueue(gRenderer, *find_tilesheet("ui"), 0, hotBar, -1, RNDRLYR_UI - 1);//Render hotbar background
	
	SDL_Rect slotRect = {0, HEIGHT - INV_spacing - 32, 32, 32};//Rect representing each slot in the hotbar
	for(int i = 0; i < INV_WIDTH; i++){//Iterate through the slots of the hotbar
		slotRect.x = (hotBar.x + 32 * i) + INV_spacing * (i + 1);
		if(selectedHotbar == i){
			SDL_Rect tmpRect = {-2, -2, 4, 4};
			tmpRect.x += slotRect.x;
			tmpRect.y += slotRect.y;
			tmpRect.w += slotRect.w;
			tmpRect.h += slotRect.h;
			AddToRenderQueue(gRenderer, *find_tilesheet("ui"), 1, tmpRect, -1, RNDRLYR_UI - 1);
		}
		AddToRenderQueue(gRenderer, *find_tilesheet("ui"), 8, slotRect, -1, RNDRLYR_UI);
		if(invArray[i].occupied == true && invArray[i].qty > 0){
			AddToRenderQueue(gRenderer, invArray[i].item.sheet, invArray[i].item.tile, slotRect, -1, RNDRLYR_INV_ITEMS);
			
			char itemqty[16];
			itoa(invArray[i].qty, itemqty, 10);
			RenderText_d(gRenderer, itemqty, slotRect.x + numOffset.x, slotRect.y + numOffset.y);
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
		
		AddToRenderQueue(gRenderer, *find_tilesheet("ui"), 0, invRect, -1, RNDRLYR_UI - 1);//Render background of inventory
		
		SDL_Point mousePoint = {mousePos.x, mousePos.y};
		mousePoint.x = (mousePoint.x - invRect.x - INV_spacing / 2) / (itemRectSize + INV_spacing);
		mousePoint.y = (mousePoint.y - invRect.y - INV_spacing / 2) / (itemRectSize + INV_spacing);
		if(mousePoint.x + 1 <= INV_WIDTH && mousePoint.y + 1 <= INV_HEIGHT){
			hoveredCell = mousePoint.x % INV_WIDTH + (mousePoint.y * INV_WIDTH);
		}
		if(SDL_PointInRect(&mousePos, &invRect) && hoveredCell >= 0 && hoveredCell < INV_WIDTH * INV_HEIGHT){
			if(mouseClicked == true){
			// if(SDL_GetTicks() > grabTime + 100){//
				// grabTime = SDL_GetTicks();//
				if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT){//LEFT CLICK
					if(strcmp(invArray[hoveredCell].item.name, mouseInv.item.name) == 0 && mouseInv.occupied == true){
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
						// printf("%s\n", mouseInv.occupied ? "true" : "false");
						mouseInv.item = invArray[hoveredCell].item;
						mouseInv.qty = invArray[hoveredCell].qty;
						mouseInv.occupied = invArray[hoveredCell].occupied;
						invArray[hoveredCell].item = tempInv.item;
						invArray[hoveredCell].qty = tempInv.qty;
						invArray[hoveredCell].occupied = tempInv.occupied;
					}
				}else if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT){//RIGHT CLICK
					if(mouseInv.qty > 0){//Check if mouse has any item
						if(invArray[hoveredCell].occupied == true || strcmp(invArray[hoveredCell].item.name, mouseInv.item.name) == 0 && invArray[hoveredCell].qty < maxStack){//Check if cell is empty or has the same item as mouse
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
		}
		
		for(int i = 0; i < INV_WIDTH * INV_HEIGHT; i++){//Inventory render loop
			int x = (i % INV_WIDTH), y = (i / INV_HEIGHT);
			invItemRect.x = (invRect.x + itemRectSize * x) + INV_spacing * (x + 1);
			invItemRect.y = (invRect.y + itemRectSize * y) + INV_spacing * (y + 1);
			
			if(SDL_PointInRect(&mousePoint, &invItemRect)){
				
				if(invArray[i].occupied == true){
					SDL_Rect itemNameDisp = {invRect.x, invRect.y - itemRectSize, strlen(invArray[i].item.name) * 14, 28};
					AddToRenderQueue(gRenderer, *find_tilesheet("ui"), 0, itemNameDisp, -1, RNDRLYR_UI - 1);//Background of item name dialogue
					RenderText_d(gRenderer, invArray[i].item.name, itemNameDisp.x + 4, itemNameDisp.y + 6);//Item name
				}
			}
			AddToRenderQueue(gRenderer, *find_tilesheet("ui"), 8, invItemRect, -1, RNDRLYR_UI);//Draw the background of each cell
			
			if(invArray[i].occupied == true && invArray[i].qty > 0){//Check if item exists in cell and render it
				AddToRenderQueue(gRenderer, invArray[i].item.sheet, invArray[i].item.tile, invItemRect, -1, RNDRLYR_INV_ITEMS);
				
				char itemqty[16];
				itoa(invArray[i].qty, itemqty, 10);
				RenderText_d(gRenderer, itemqty, invItemRect.x + numOffset.x, invItemRect.y + numOffset.y);//Item amount
			}else{
				invArray[i].occupied = false;
				invArray[i].qty = 0;
			}
		}
		//Drawing the mouse inventory
		if(mouseInv.occupied == true && mouseInv.qty > 0){
			SDL_Rect mouseItem = {mousePos.x - 16, mousePos.y - 16, itemRectSize, itemRectSize};
			AddToRenderQueue(gRenderer, mouseInv.item.sheet, mouseInv.item.tile, mouseItem, 255, RNDRLYR_INV_ITEMS);
			
			char itemqty[16];
			itoa(mouseInv.qty, itemqty, 10);
			RenderText_d(gRenderer, itemqty, mouseItem.x + numOffset.x, mouseItem.y + numOffset.y);//Draw item quantity for each item
		}
	}else{
		uiInteractMode = false;
	}
}


int INV_WriteCell(char *mode, int cell, int itemQty, ItemComponent item){
	if(cell > INV_HEIGHT * INV_WIDTH){
		printf("Error: Item location out of bounds!\n");
		return 1;
	}
		// printf("im here!\n");

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
			if(strcmp(invArray[cell].item.name, item.name) == 0 && invArray[cell].qty <= maxStack){//Check if item is of different type or exceeding limit
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
			if(strcmp(invArray[cell].item.name, item.name) == 0){
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

int INV_FindItem(ItemComponent itemNum){
	int itemQtyFound = 0;
	for(int i = 0; i < INV_WIDTH * INV_HEIGHT; i++){
		if(strcmp(invArray[i].item.name, itemNum.name) == 0 && invArray[i].qty > 0){
			return i;
		}
	}
	return -1;
}
int INV_FindEmpty(ItemComponent item){
	for(int i = 0; i < INV_WIDTH * INV_HEIGHT; i++){
		if(strcmp(invArray[i].item.name, item.name) == 0 && invArray[i].qty <= maxStack){
			return i;
		}
	}
	for(int i = 0; i < INV_WIDTH * INV_HEIGHT; i++){
		if(invArray[i].occupied == false){
			return i;
		}
	}
	printf("Inventory Full!");
	return -1;
}