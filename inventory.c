#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include "headers/DataTypes.h"
#include "headers/initialize.h"
#include "headers/data.h"
#include "headers/drawFunctions.h"
#include "headers/tileMap.h"
#include "headers/inventory.h"

// const int INV_WIDTH = 5;
// const int INV_HEIGHT = 4;
// enum INV_PARAMS {INV_WIDTH = 4, INV_HEIGHT = 4};
int invArray[INV_HEIGHT * INV_WIDTH][2];

int selectedHotbar = 0;

int maxStack = 99;

Vector2 numOffset = {-2, 16};
int mouseInv[2] = {-1, 0};
bool showInv = false;
INV_Item itemData[64];
INV_Recipe recipes[64];
int numberOfRecipes = 0;

void INV_Init(){
	int invPos = 0;
	for(int y = 0; y < INV_HEIGHT; y++){
		for(int x = 0; x < INV_WIDTH; x++){
			invPos++;
			invArray[invPos][0] = -1;
			invArray[invPos][1] = 0;
		}
	}
	// INV_WriteCell("set", INV_FindEmpty(-1), 1, 0);
	// INV_WriteCell("set", INV_FindEmpty(-1), 1, 1);
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
	int i = 0;
	char heading[64];
	
	while(fgets(buffer, sizeof(buffer), file)){
		if(buffer[0] != '\n'){
			if(buffer[0] == '	'){
				strshft_l(buffer, 1);
				
				if(strcmp(heading, "BLOCKS") == 0){//Check if iterating through blocks
					// char id[16];
					// strcpy(id, strtok(buffer, ":"));
					// strcpy(id, strtok(NULL, "|"));
					// printf("%s\n", id);
					// strshft_l(id, 3);	
					// printf("%d\n", atoi(id));
					// printf("%s\n", id);
					
				}else{
					strcpy(itemData[i].name, strtok(buffer, ":"));
					strcpy(itemData[i].description, strtok(NULL, "|"));
				}
				i++;
				// printf("Current heading: %s\n", heading);
			}else if(buffer[0] == ':' && buffer[1] == ':'){
				strshft_l(buffer, 2);
				i = 0;
				// buffer[strlen(buffer) - 1] = ' ';
				// buffer[strlen(buffer) - 2] = ' ';
				// printf("Reading %s\n", buffer);
				strcpy(heading, buffer);
				// printf("Reading: %s\n", buffer);
			}/*else  if(buffer[0] == '/' && buffer[1] == '/'){
				
			} */
		}
		// printf("%s >> %s\n", itemData[i].name, itemData[i].description);
		// printf("%s >> %s\n", itemData[0].name, itemData[0].description);
	}
	// printf("%s\n", itemData[0].name);
	INV_InitRecipes();
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

void INV_DrawInv(){
	int INV_spacing = 8;
	SDL_Rect invItemRect = {0, 0, 32, 32};
	
	//Drawing the hotbar
	SDL_Rect hotBar = {WIDTH / 2 - INV_WIDTH * 32 + (INV_WIDTH + 1) * INV_spacing, HEIGHT - INV_spacing * 2 - 32, // ->
	INV_WIDTH * 32 + (INV_WIDTH + 1) * INV_spacing, INV_spacing * 2 + 32};
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 128);
	// SDL_RenderFillRect(gRenderer, &hotBar);
	AddToRenderQueue(gRenderer, uiSheet, 0, hotBar, RNDRLYR_UI - 1);
	
	SDL_Rect slotRect = {0, HEIGHT - INV_spacing - 32, 32, 32};
	for(int i = 0; i < INV_WIDTH; i++){
		slotRect.x = (hotBar.x + 32 * i) + INV_spacing * (i + 1);
		if(selectedHotbar == i){
			SDL_Rect tmpRect = {-2, -2, 4, 4};
			tmpRect.x += slotRect.x;
			tmpRect.y += slotRect.y;
			tmpRect.w += slotRect.w;
			tmpRect.h += slotRect.h;
			AddToRenderQueue(gRenderer, uiSheet, 1, tmpRect, RNDRLYR_UI - 1);
		}
		AddToRenderQueue(gRenderer, uiSheet, 8, slotRect, RNDRLYR_UI);
		if(invArray[i][0] > -1 && invArray[i][1] > 0){
			AddToRenderQueue(gRenderer, itemSheet, invArray[i][0], slotRect, RNDRLYR_INV_ITEMS);
			
			char itemqty[16];
			itoa(invArray[i][1], itemqty, 10);
			RenderText_d(gRenderer, itemqty, slotRect.x + numOffset.x, slotRect.y + numOffset.y);
		}
	}
	
	//Drawing the main inventory
	if(showInv){
		SDL_Rect invRect = {WIDTH / 2 - INV_WIDTH * 32 + (INV_WIDTH + 1) * INV_spacing, HEIGHT - INV_HEIGHT * 32 + (INV_HEIGHT + 1) * INV_spacing - 132, // ->
		INV_WIDTH * 32 + (INV_WIDTH + 1) * INV_spacing, INV_HEIGHT * 32 + (INV_HEIGHT + 1) * INV_spacing};
		
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
		AddToRenderQueue(gRenderer, uiSheet, 0, invRect, RNDRLYR_UI - 1);
		
		for(int i = 0; i < INV_WIDTH * INV_HEIGHT; i++){
			int x = (i % INV_WIDTH), y = (i / INV_HEIGHT);
			invItemRect.x = (invRect.x + 32 * x) + INV_spacing * (x + 1);
			invItemRect.y = (invRect.y + 32 * y) + INV_spacing * (y + 1);
			
			SDL_GetMouseState(&mousePos.x, &mousePos.y);
			SDL_Point mousePoint = {mousePos.x, mousePos.y};
			if(SDL_PointInRect(&mousePoint, &invItemRect)){
				if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT){//LEFT CLICK
					if(SDL_PollEvent(&e) == 1){
						if(invArray[i][0] == mouseInv[0]){
							INV_WriteCell("add", i, mouseInv[1], mouseInv[0]);
							mouseInv[0] = -1;
							mouseInv[1] = 0;
						}else{
							//Swap clicked item with held item
							int tempInv[2] = {mouseInv[0], mouseInv[1]};
							mouseInv[0] = invArray[i][0];
							mouseInv[1] = invArray[i][1];
							invArray[i][0] = tempInv[0];
							invArray[i][1] = tempInv[1];
						}
					}
				}else if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT){//RIGHT CLICK
					if(SDL_PollEvent(&e) == 1){
						if(mouseInv[1] > 0){//Check if mouse has any item
							if(invArray[i][0] == -1 || invArray[i][0] == mouseInv[0]){//Check if cell is empty or has the same item as mouse
								if(mouseInv[1] > 1){//If there are multiple items in mouseInv add one to inv cell
									INV_WriteCell("add", i, 1, mouseInv[0]);
									mouseInv[1]--;
								}else{//If there was only 1 empty mouse slot
									INV_WriteCell("add", i, 1, mouseInv[0]);
									mouseInv[0] = -1;
									mouseInv[1] = 0;
								}
							}
						}else if(invArray[i][1] > 1){//Otherwise take half the items in that inventory cell
							mouseInv[0] = invArray[i][0];
							mouseInv[1] = invArray[i][1] / 2;
							INV_WriteCell("sub", i, invArray[i][1] / 2, invArray[i][0]);
						}
					}
				}
				if(invArray[i][0] > -1){
					SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
					SDL_Rect itemNameDisp = {invRect.x, invRect.y - 32, strlen(itemData[invArray[i][0]].name) * 14, 28};
					AddToRenderQueue(gRenderer, uiSheet, 0, itemNameDisp, RNDRLYR_UI - 1);
					SDL_Rect woahR = {0, 0, 100, 100};
					AddToRenderQueue(gRenderer, uiSheet, 0, woahR, RNDRLYR_UI - 1);
					RenderText_d(gRenderer, itemData[invArray[i][0]].name, itemNameDisp.x + 4, itemNameDisp.y + 6);					
				}
			}
			AddToRenderQueue(gRenderer, uiSheet, 8, invItemRect, RNDRLYR_UI);//Draw the background of each cell
			
			if(invArray[i][0] > -1 && invArray[i][1] > 0){//Check if item exists in cell and render it
				AddToRenderQueue(gRenderer, itemSheet, invArray[i][0], invItemRect, RNDRLYR_INV_ITEMS);
				
				char itemqty[16];
				itoa(invArray[i][1], itemqty, 10);
				RenderText_d(gRenderer, itemqty, invItemRect.x + numOffset.x, invItemRect.y + numOffset.y);//Item amount
			}else{
				invArray[i][0] = -1;
				invArray[i][1] = 0;
			}
		}
		//Drawing the mouse inventory
		if(mouseInv[0] > -1 && mouseInv[1] > 0){
			SDL_Rect mouseItem = {mousePos.x - 16, mousePos.y - 16, 32, 32};
			AddToRenderQueue(gRenderer, itemSheet, mouseInv[0], mouseItem, RNDRLYR_INV_ITEMS);
			
			char itemqty[16];
			itoa(mouseInv[1], itemqty, 10);
			RenderText_d(gRenderer, itemqty, mouseItem.x + numOffset.x, mouseItem.y + numOffset.y);
		}
	}
	// printf("\n");
	// printf("\n");
}

int INV_WriteCell(char *mode, int cell, int itemQty, int itemNum){
	printf("%s %d, to %d\n", mode, itemQty, cell);
	if(cell > INV_HEIGHT * INV_WIDTH){
		printf("Error: Item location out of bounds!\n");
		return 1;
	}
	if(itemQty != NULL && itemQty != 0 && itemNum > -1){
		
		if(strcmp(mode, "set") == 0){
			invArray[cell][0] = itemNum;
			if(itemQty < maxStack){
				invArray[cell][1] = itemQty;
			}else{
				invArray[cell][1] = maxStack;
			}
		}else if(strcmp(mode, "add") == 0){
			invArray[cell][0] = itemNum;
			if(invArray[cell][0] == itemNum && invArray[cell][1] < maxStack){//Check if item is of different type or exceeding limit
				invArray[cell][1] += itemQty;
			}else{
				printf("Error: INV_WriteCell Attempting to add to full cell (Use 'set' instead of 'add')\n");
				return 1;
			}
			return 0;
		}else if(strcmp(mode, "sub") == 0){
			if(invArray[cell][0] == itemNum){
				if(invArray[cell][1] > 0){
					invArray[cell][1] -= itemQty;
				}else{
					invArray[cell][1] = 0;
					invArray[cell][0] = -1;
				}
			}
		}
	}else{
		invArray[cell][0] = -1;
		invArray[cell][1] = 0;
	}
	return 0;
}


int INV_ReadCell(char *mode, int cell){
	// return invArray[cell / INV_HEIGHT][cell % INV_WIDTH][0];
	return invArray[cell][0];
}

int INV_FindItem(int itemNum){
	int itemQtyFound = 0;
	for(int i = 0; i < INV_WIDTH * INV_HEIGHT; i++){
		if(invArray[i][0] == itemNum && invArray[i][1] > 0){
			return i;
		}
	}
	return -1;
}
int INV_FindEmpty(int id){
	for(int i = 0; i < INV_WIDTH * INV_HEIGHT; i++){
		if(invArray[i][0] == id && invArray[i][1] <= maxStack){
			return i;
		}
	}
	for(int i = 0; i < INV_WIDTH * INV_HEIGHT; i++){
		if(invArray[i][0] == -1){
			return i;
		}
	}
	printf("Inventory Full!");
	return -1;
}