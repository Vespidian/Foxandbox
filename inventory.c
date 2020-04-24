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
	
	// invArray[9][0] = 2;
	// invArray[9][1] = 1;
	
	INV_WriteCell("set", 11, 0, -1);
	
	// printf("%d", INV_ReadCell("b", 11));
}

int mouseInv[2] = {-1, 0};
bool showInv = false;

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
			RenderText(gRenderer, itemqty, slotRect.x + numOffset.x, slotRect.y + numOffset.y);
		}
	}
	
	//Drawing the main inventory
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
				if(SDL_PointInRect(&mousePoint, &invItemRect) && e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT){//LEFT CLICK
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
				}else if(SDL_PointInRect(&mousePoint, &invItemRect) && e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT){//RIGHT CLICK
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
				RenderTextureFromSheet(gRenderer, uiSheet, 0, invItemRect);//Draw the background of each cell
				
				if(invArray[invPos][0] > -1 && invArray[invPos][1] > 0){//Check if item exists in cell and render it
					RenderTextureFromSheet(gRenderer, itemSheet, invArray[invPos][0], invItemRect);
					
					char itemqty[16];
					itoa(invArray[invPos][1], itemqty, 10);
					RenderText(gRenderer, itemqty, invItemRect.x + numOffset.x, invItemRect.y + numOffset.y);//Item amount
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
			RenderText(gRenderer, itemqty, mouseItem.x + numOffset.x, mouseItem.y + numOffset.y);
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

int INV_FindEmpty(int type){
	int invPos = 0;
	for(int y = 0; y < INV_HEIGHT; y++){
		for(int x = 0; x < INV_WIDTH; x++){
			invPos++;
			if((invArray[invPos][0] == type && invArray[invPos][1] <= maxStack) || (invArray[invPos][0] == -1)){
				// if(invArray[y][x][1] == amount){
					return invPos;
				// }
			}
		}
	}
}