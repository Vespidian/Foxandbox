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

int selectedHotbar = 1;

int maxStack = 99;

Vector2 numOffset = {-2, 16};
int mouseInv[2] = {-1, 0};
bool showInv = false;
INV_Item itemData[64];

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
}

int ReadItemData(){
	FILE *file = fopen("data/items.dat", "r");
	if(file == NULL){
		return 1;
	}
	char buffer[512];
	int i = 0;
	
	while(fgets(buffer, sizeof(buffer), file)){		
		strcpy(itemData[i].name, strtok(buffer, ":"));
		strcpy(itemData[i].description, strtok(NULL, "\n"));
		// printf("%s >> %s\n", itemData[i].name, itemData[i].description);
		// printf("%s >> %s\n", itemData[0].name, itemData[0].description);
		i++;
	}
	
	// printf("%s\n", itemData[0].name);
	
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
	
	while(fgets(buffer, sizeof(buffer), file)){
		
	}
}

void INV_DrawInv(){
	int INV_spacing = 8;
	SDL_Rect invItemRect = {0, 0, 32, 32};
	
	//Drawing the hotbar
	SDL_Rect hotBar = {WIDTH / 2 - INV_WIDTH * 32 + (INV_WIDTH + 1) * INV_spacing, HEIGHT - INV_spacing * 2 - 32, // ->
	INV_WIDTH * 32 + (INV_WIDTH + 1) * INV_spacing, INV_spacing * 2 + 32};
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 128);
	SDL_RenderFillRect(gRenderer, &hotBar);
	
	SDL_Rect slotRect = {0, HEIGHT - INV_spacing - 32, 32, 32};
	for(int i = 0; i < INV_WIDTH; i++){
		slotRect.x = (hotBar.x + 32 * i) + INV_spacing * (i + 1);
		if(selectedHotbar == i + 1){
			SDL_Rect tmpRect = {-2, -2, 4, 4};
			tmpRect.x += slotRect.x;
			tmpRect.y += slotRect.y;
			tmpRect.w += slotRect.w;
			tmpRect.h += slotRect.h;
			SDL_SetRenderDrawColor(gRenderer, 200, 0, 0, 64);
			SDL_RenderFillRect(gRenderer, &tmpRect);
			// SDL_RenderDrawRect(gRenderer, &slotRect);
		}
		RenderTextureFromSheet(gRenderer, uiSheet, 0, slotRect);
		if(invArray[i+1][0] > -1 && invArray[i+1][1] > 0){
			RenderTextureFromSheet(gRenderer, itemSheet, invArray[i+1][0], slotRect);
			
			char itemqty[16];
			itoa(invArray[i+1][1], itemqty, 10);
			RenderText_d(gRenderer, itemqty, slotRect.x + numOffset.x, slotRect.y + numOffset.y);
		}
	}
	
	//Drawing the main inventory
	/* for(int x = 0; x < 64; x++){
		// printf("%s\n", itemData[x].description);
		printf("%s\n", itemData[x].name);
	} */
	if(showInv){
		/*SDL_Rect invRect = {WIDTH / 2 - INV_WIDTH * 32 + (INV_WIDTH + 1) * INV_spacing, HEIGHT / 2 + HEIGHT / 4, // ->
		INV_WIDTH * 32 + (INV_WIDTH + 1) * INV_spacing, INV_HEIGHT * 32 + (INV_HEIGHT + 1) * INV_spacing};*/
		
		SDL_Rect invRect = {WIDTH / 2 - INV_WIDTH * 32 + (INV_WIDTH + 1) * INV_spacing, HEIGHT - INV_HEIGHT * 32 + (INV_HEIGHT + 1) * INV_spacing - 132, // ->
		INV_WIDTH * 32 + (INV_WIDTH + 1) * INV_spacing, INV_HEIGHT * 32 + (INV_HEIGHT + 1) * INV_spacing};
		
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 128);
		SDL_RenderFillRect(gRenderer, &invRect);
		
		int invPos = 0;
		for(int y = 0; y < INV_HEIGHT; y++){
			for(int x = 0; x < INV_WIDTH; x++){
				invPos++;
				invItemRect.x = (invRect.x + 32 * x) + INV_spacing * (x + 1);
				invItemRect.y = (invRect.y + 32 * y) + INV_spacing * (y + 1);
				
				SDL_GetMouseState(&mousePos.x, &mousePos.y);
				SDL_Point mousePoint = {mousePos.x, mousePos.y};
				if(SDL_PointInRect(&mousePoint, &invItemRect)){
					if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT){//LEFT CLICK
						if(SDL_PollEvent(&e) == 1){
							if(invArray[invPos][0] == mouseInv[0]){
								INV_WriteCell("add", invPos - 1, mouseInv[1], mouseInv[0]);
								mouseInv[0] = -1;
								mouseInv[1] = 0;
							}else{
								//Swap clicked item with held item
								int tempInv[2] = {mouseInv[0], mouseInv[1]};
								mouseInv[0] = invArray[invPos][0];
								mouseInv[1] = invArray[invPos][1];
								invArray[invPos][0] = tempInv[0];
								invArray[invPos][1] = tempInv[1];
							}
						}
					}else if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT){//RIGHT CLICK
						if(SDL_PollEvent(&e) == 1){
							if(mouseInv[1] > 0){//Check if mouse has any item
								if(invArray[invPos][0] == -1 || invArray[invPos][0] == mouseInv[0]){
									if(mouseInv[1] > 1){
										INV_WriteCell("add", invPos - 1, 1, mouseInv[0]);
										mouseInv[1]--;
									}else{
										INV_WriteCell("add", invPos - 1, 1, mouseInv[0]);
										mouseInv[0] = -1;
										mouseInv[1] = 0;
									}
								}
							}else if(invArray[invPos][1] > 1){
								mouseInv[0] = invArray[invPos][0];
								mouseInv[1] = invArray[invPos][1] / 2;
								INV_WriteCell("sub", invPos - 1, invArray[invPos][1] / 2, invArray[invPos][0]);
							}
						}
					}
					if(invArray[invPos][0] > -1){
						SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
						SDL_Rect itemNameDisp = {invRect.x, invRect.y - 32, strlen(itemData[invArray[invPos][0]].name) * 14, 28};
						SDL_RenderFillRect(gRenderer, &itemNameDisp);
						RenderText_d(gRenderer, itemData[invArray[invPos][0]].name, itemNameDisp.x + 4, itemNameDisp.y + 6);
						
						// SDL_Color tmpBlue = {20, 20, 255, 0xff};
						// RenderText(gRenderer, itemData[invArray[invPos][0]].description, itemNameDisp.x + 4, itemNameDisp.y - 20, tmpBlue);
					}
				}
				RenderTextureFromSheet(gRenderer, uiSheet, 0, invItemRect);//Draw the background of each cell
				
				if(invArray[invPos][0] > -1 && invArray[invPos][1] > 0){//Check if item exists in cell and render it
					RenderTextureFromSheet(gRenderer, itemSheet, invArray[invPos][0], invItemRect);
					
					char itemqty[16];
					itoa(invArray[invPos][1], itemqty, 10);
					RenderText_d(gRenderer, itemqty, invItemRect.x + numOffset.x, invItemRect.y + numOffset.y);//Item amount
				}else{
					invArray[invPos][0] = -1;
					invArray[invPos][1] = 0;
				}
			}
		}
		//Drawing the mouse inventory
		if(mouseInv[0] > -1 && mouseInv[1] > 0){
			SDL_Rect mouseItem = {mousePos.x - 16, mousePos.y - 16, 32, 32};
			RenderTextureFromSheet(gRenderer, itemSheet, mouseInv[0], mouseItem);
			
			char itemqty[16];
			itoa(mouseInv[1], itemqty, 10);
			RenderText_d(gRenderer, itemqty, mouseItem.x + numOffset.x, mouseItem.y + numOffset.y);
		}
	}
	// printf("\n");
	// printf("\n");
}

int INV_WriteCell(char *mode, int cell, int itemQty, int itemNum){
	if(cell > INV_HEIGHT * INV_WIDTH){
		printf("Error: Item location out of bounds!");
		return 1;
	}
	if(itemQty != NULL && itemQty != 0){
		invArray[cell + 1][0] = itemNum;
		
		if(strcmp(mode, "set") == 0){
			if(itemQty > maxStack){
				invArray[cell + 1][1] = maxStack;
			}else{
				invArray[cell + 1][1] = itemQty;
			}
		}else if(strcmp(mode, "add") == 0){
			if(invArray[cell + 1][0] == itemNum && invArray[cell + 1][1] < maxStack){//Check if item is of different type or exceeding limit
				invArray[cell + 1][1] += itemQty;
			}else{
				printf("Error: INV_WriteCell Atemping to add to full cell (Use 'set' instead of 'add')");
				return 1;
			}
			return 0;
		}else if(strcmp(mode, "sub") == 0){
			if(invArray[cell + 1][1] > 0){
				invArray[cell + 1][1] -= itemQty;
			}
		}
	}else{
		invArray[cell + 1][0] = -1;
		invArray[cell + 1][1] = 0;
	}
	return 0;
}


int INV_ReadCell(char *mode, int cell){
	// return invArray[cell / INV_HEIGHT][cell % INV_WIDTH][0];
	return invArray[cell + 1][0];
}

int INV_FindItem(int itemNum, int minItemQty){
	if(minItemQty < 1){
		printf("Error: INV_FindItem called with a minimum quantity of less than 1\n");
		return 1;
	}
	int itemQtyFound = 0;
	for(int i = 0; i < INV_WIDTH * INV_HEIGHT; i++){
		if(invArray[i][0] == itemNum){
			// itemQtyFound += invArray[i][1];
			itemQtyFound = invArray[i][1];
			if(itemQtyFound >= minItemQty){
				return i;
			}
		}
	}
}
int INV_FindEmpty(int type){
	for(int i = 0; i < INV_WIDTH * INV_HEIGHT; i++){
		if(invArray[i][0] == type && invArray[i][1] <= maxStack){
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