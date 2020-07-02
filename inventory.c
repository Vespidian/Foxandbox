#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "headers/DataTypes.h"
#include "headers/ECS.h"

#include "headers/initialize.h"
#include "headers/renderSystems.h"
#include "headers/data.h"
#include "headers/tileMap.h"
#include "headers/inventory.h"

enum INV_SECTIONS {ITM_SECT, QTY_SECT};
int invArray[INV_HEIGHT * INV_WIDTH][2];

int selectedHotbar = 0;

int INV_spacing = 8;
int maxStack = 99;

Vector2 numOffset = {-2, 16};
int mouseInv[2] = {-1, 0};
bool showInv = false;
// INV_ItemComponent itemData[64];
INV_ItemComponent *itemData;
INV_BlockComponent blockData[128];
INV_RecipeComponent recipes[64];
int numberOfRecipes = 0;
int grabTime = 0;

void INV_Init(){
	int invPos = 0;
	for(int y = 0; y < INV_HEIGHT; y++){
		for(int x = 0; x < INV_WIDTH; x++){
			invPos++;
			invArray[invPos][0] = -1;
			invArray[invPos][1] = 0;
		}
	}
	INV_WriteCell("set", 0, 2, 1);
	INV_WriteCell("set", 12, 90, 2);
	INV_WriteCell("set", 8, 1, 3);
	INV_WriteCell("set", 3, 1, 1);
	INV_WriteCell("set", 4, 1, 0);
	INV_WriteCell("set", 11, 0, -1);
	INV_WriteCell("set", 14, 20, 4);
}

int ReadItemData(){
	FILE *file = fopen("data/items.dat", "r");
	if(file == NULL){
		return 1;
	}
	char buffer[512];
	char declarationType[64];
	
	
	itemData = malloc(sizeof(INV_ItemComponent *));
	
	int itemCounter = 0;
	while(fgets(buffer, sizeof(buffer), file)){
		if(buffer[0] != '\n'){
			if(buffer[0] == '\t' && buffer[1] != '\n'){
				strcpy(buffer, strshft_l(buffer, 1));
				
				// printf("%s	-> %d\n", declarationType, itemCounter);
				/*if(strcmp(declarationType, "CRAFTING_INGREDIENTS") == 0){
					itemData = realloc(itemData, (itemCounter + 1) * sizeof(INV_ItemComponent *));
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
				itemCounter++;
				// printf("Current declarationType: %s\n", declarationType);
			}else if(buffer[0] == ':' && buffer[1] == ':'){
				strcpy(buffer, strshft_l(buffer, 2));
				// strshft_l(buffer, 2);
				itemCounter = 0;
				buffer[strlen(buffer) - 1] = '\0';
				strcpy(declarationType, buffer);
			}/*else  if(buffer[0] == '/' && buffer[1] == '/'){
				
			} */
		}
		// printf("%s >> %s\n", itemData[itemCounter].name, itemData[itemCounter].description);
		// printf("%s >> %s\n", itemData[0].name, itemData[0].description);
	}
	// printf("%s\n", itemData[0].name);
	// INV_InitRecipes();
	fclose(file);
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
		if(invArray[i][ITM_SECT] > -1 && invArray[i][QTY_SECT] > 0){
			AddToRenderQueue(gRenderer, itemSheet, invArray[i][0], slotRect, -1, RNDRLYR_INV_ITEMS);
			
			char itemqty[16];
			itoa(invArray[i][1], itemqty, 10);
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
					if(invArray[hoveredCell][ITM_SECT] == mouseInv[0]){
						int mouseRemainder = INV_WriteCell("add", hoveredCell, mouseInv[1], mouseInv[0]);
						if(mouseRemainder == 0){
							mouseInv[0] = -1;
							mouseInv[1] = 0;
						}else{
							mouseInv[1] = mouseRemainder;
						}
					}else{//Swap clicked item with held item
						int tempInv[2] = {mouseInv[0], mouseInv[1]};
						mouseInv[0] = invArray[hoveredCell][ITM_SECT];
						mouseInv[1] = invArray[hoveredCell][1];
						invArray[hoveredCell][ITM_SECT] = tempInv[0];
						invArray[hoveredCell][QTY_SECT] = tempInv[1];
					}
				}else if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT){//RIGHT CLICK
					if(mouseInv[1] > 0){//Check if mouse has any item
						if(invArray[hoveredCell][ITM_SECT] == -1 || invArray[hoveredCell][ITM_SECT] == mouseInv[0] && invArray[hoveredCell][QTY_SECT] < maxStack){//Check if cell is empty or has the same item as mouse
							if(mouseInv[1] > 1){//If there are multiple items in mouseInv add one to inv cell
								INV_WriteCell("add", hoveredCell, 1, mouseInv[0]);
								mouseInv[1]--;
							}else{//If there was only 1, empty mouse slot
								INV_WriteCell("add", hoveredCell, 1, mouseInv[0]);
								mouseInv[0] = -1;
								mouseInv[1] = 0;
							}
						}
					}else if(invArray[hoveredCell][QTY_SECT] > 1){//Otherwise take half the items in that inventory cell
						mouseInv[0] = invArray[hoveredCell][ITM_SECT];
						mouseInv[1] = invArray[hoveredCell][QTY_SECT] / 2;
						INV_WriteCell("sub", hoveredCell, invArray[hoveredCell][QTY_SECT] / 2, invArray[hoveredCell][ITM_SECT]);
					}
				}
			}
		}
		
		for(int i = 0; i < INV_WIDTH * INV_HEIGHT; i++){//Inventory render loop
			int x = (i % INV_WIDTH), y = (i / INV_HEIGHT);
			invItemRect.x = (invRect.x + itemRectSize * x) + INV_spacing * (x + 1);
			invItemRect.y = (invRect.y + itemRectSize * y) + INV_spacing * (y + 1);
			
			if(SDL_PointInRect(&mousePoint, &invItemRect)){
				
				if(invArray[i][ITM_SECT] > -1){
					SDL_Rect itemNameDisp = {invRect.x, invRect.y - itemRectSize, strlen(itemData[invArray[i][ITM_SECT]].name) * 14, 28};
					AddToRenderQueue(gRenderer, *find_tilesheet("ui"), 0, itemNameDisp, -1, RNDRLYR_UI - 1);//Background of item name dialogue
					RenderText_d(gRenderer, itemData[invArray[i][ITM_SECT]].name, itemNameDisp.x + 4, itemNameDisp.y + 6);//Item name
				}
			}
			AddToRenderQueue(gRenderer, *find_tilesheet("ui"), 8, invItemRect, -1, RNDRLYR_UI);//Draw the background of each cell
			
			if(invArray[i][ITM_SECT] > -1 && invArray[i][QTY_SECT] > 0){//Check if item exists in cell and render it
				AddToRenderQueue(gRenderer, itemSheet, invArray[i][ITM_SECT], invItemRect, -1, RNDRLYR_INV_ITEMS);
				
				char itemqty[16];
				itoa(invArray[i][1], itemqty, 10);
				RenderText_d(gRenderer, itemqty, invItemRect.x + numOffset.x, invItemRect.y + numOffset.y);//Item amount
			}else{
				invArray[i][ITM_SECT] = -1;
				invArray[i][QTY_SECT] = 0;
			}
		}
		//Drawing the mouse inventory
		if(mouseInv[0] > -1 && mouseInv[1] > 0){
			SDL_Rect mouseItem = {mousePos.x - 16, mousePos.y - 16, itemRectSize, itemRectSize};
			AddToRenderQueue(gRenderer, itemSheet, mouseInv[0], mouseItem, -1, RNDRLYR_INV_ITEMS);
			
			char itemqty[16];
			itoa(mouseInv[1], itemqty, 10);
			RenderText_d(gRenderer, itemqty, mouseItem.x + numOffset.x, mouseItem.y + numOffset.y);//Draw item quantity for each item
		}
	}else{
		uiInteractMode = false;
	}
}


int INV_WriteCell(char *mode, int cell, int itemQty, int itemNum){
	if(cell > INV_HEIGHT * INV_WIDTH){
		printf("Error: Item location out of bounds!\n");
		return 1;
	}
	if(itemQty != NULL && itemQty != 0 && itemNum > -1){
		
		if(strcmp(mode, "set") == 0){
			invArray[cell][ITM_SECT] = itemNum;
			if(itemQty < maxStack){
				invArray[cell][QTY_SECT] = itemQty;
			}else{
				invArray[cell][QTY_SECT] = maxStack;
			}
		}else if(strcmp(mode, "add") == 0){
			invArray[cell][ITM_SECT] = itemNum;
			if(invArray[cell][ITM_SECT] == itemNum && invArray[cell][QTY_SECT] <= maxStack){//Check if item is of different type or exceeding limit
				if(invArray[cell][QTY_SECT] + itemQty > maxStack){//Check if adding item will cause cell to exceed maxStack size
					int remainder = (itemQty - (maxStack - invArray[cell][1]));
					invArray[cell][QTY_SECT] += maxStack - invArray[cell][1];//Fill the cell
					return remainder;//Return the remainder if the stack is full
				}else{
					invArray[cell][QTY_SECT] += itemQty;
					return 0;
				}
			}
		}else if(strcmp(mode, "sub") == 0){
			if(invArray[cell][ITM_SECT] == itemNum){
				if(invArray[cell][QTY_SECT] > 0){
					invArray[cell][QTY_SECT] -= itemQty;
				}else{
					invArray[cell][QTY_SECT] = 0;
					invArray[cell][ITM_SECT] = -1;
				}
			}
		}
	}else{
		invArray[cell][ITM_SECT] = -1;
		invArray[cell][QTY_SECT] = 0;
	}
	return 0;
}

int INV_FindItem(int itemNum){
	int itemQtyFound = 0;
	for(int i = 0; i < INV_WIDTH * INV_HEIGHT; i++){
		if(invArray[i][ITM_SECT] == itemNum && invArray[i][QTY_SECT] > 0){
			return i;
		}
	}
	return -1;
}
int INV_FindEmpty(int id){
	for(int i = 0; i < INV_WIDTH * INV_HEIGHT; i++){
		if(invArray[i][ITM_SECT] == id && invArray[i][QTY_SECT] <= maxStack){
			return i;
		}
	}
	for(int i = 0; i < INV_WIDTH * INV_HEIGHT; i++){
		if(invArray[i][ITM_SECT] == -1){
			return i;
		}
	}
	printf("Inventory Full!");
	return -1;
}