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
	INV_WriteCell("set", 0, 1, 1);
	INV_WriteCell("set", 12, 1, 2);
	INV_WriteCell("set", 8, 1, 3);
	INV_WriteCell("set", 3, 1, 1);
	
	// invArray[9][0] = 2;
	// invArray[9][1] = 1;
	
	INV_WriteCell("set", 11, 0, -1);
	
	// printf("%d", INV_ReadCell("b", 11));
}

int mouseInv[2] = {-1, 0};
bool showInv = false;

void INV_DrawInv(){
	int spacing = 8;
	SDL_Rect invItemRect = {0, 0, 32, 32};
	
	//Drawing the hotbar
	SDL_Rect hotBar = {WIDTH / 2 - INV_WIDTH * 32 + (INV_WIDTH + 1) * spacing, HEIGHT - spacing * 2 - 32, // ->
	INV_WIDTH * 32 + (INV_WIDTH + 1) * spacing, spacing * 2 + 32};
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 128);
	SDL_RenderFillRect(gRenderer, &hotBar);
	
	SDL_Rect slotRect = {0, HEIGHT - spacing - 32, 32, 32};
	for(int i = 0; i < INV_WIDTH; i++){
		slotRect.x = (hotBar.x + 32 * i) + spacing * (i + 1);
		if(selectedHotbar == i + 1){
			SDL_SetRenderDrawColor(gRenderer, 200, 0, 0, 64);
			SDL_RenderFillRect(gRenderer, &slotRect);
			// SDL_RenderDrawRect(gRenderer, &slotRect);
		}
		RenderTextureFromSheet(gRenderer, uiSheet, 0, slotRect);
		if(invArray[i+1][0] > -1 && invArray[i][1+1] > 0){
			RenderTextureFromSheet(gRenderer, itemSheet, invArray[i+1][0], slotRect);
		}
	}
	
	
	//Drawing the main inventory
	if(showInv){
		SDL_Rect invRect = {WIDTH / 2 - INV_WIDTH * 32 + (INV_WIDTH + 1) * spacing, HEIGHT / 2 + HEIGHT / 4, // ->
		INV_WIDTH * 32 + (INV_WIDTH + 1) * spacing, INV_HEIGHT * 32 + (INV_HEIGHT + 1) * spacing};
		
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 128);
		SDL_RenderFillRect(gRenderer, &invRect);
		SDL_SetRenderDrawColor(gRenderer, 200, 200, 200, 64);
		
		int invPos = 0;
		for(int y = 0; y < INV_HEIGHT; y++){
			for(int x = 0; x < INV_WIDTH; x++){
				invPos++;
				invItemRect.x = (invRect.x + 32 * x) + spacing * (x + 1);
				invItemRect.y = (invRect.y + 32 * y) + spacing * (y + 1);
				
				SDL_GetMouseState(&mousePos.x, &mousePos.y);
				SDL_Point mousePoint = {mousePos.x, mousePos.y};
				if(SDL_PointInRect(&mousePoint, &invItemRect) && e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT){
					if(SDL_PollEvent(&e) == 1){
						//Swap clicked item with item held by mouse
						int tempInv[2] = {mouseInv[0], mouseInv[1]};
						mouseInv[0] = invArray[invPos][0];
						mouseInv[1] = invArray[invPos][1];
						invArray[invPos][0] = tempInv[0];
						invArray[invPos][1] = tempInv[1];
					}
				}
				// SDL_RenderFillRect(gRenderer, &invItemRect);
				RenderTextureFromSheet(gRenderer, uiSheet, 0, invItemRect);
				
				if(invArray[invPos][0] > -1 && invArray[invPos][1] > 0){
					// SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 128);	//temp
					// SDL_RenderDrawRect(gRenderer, &invItemRect);			//temp
					// SDL_SetRenderDrawColor(gRenderer, 200, 200, 200, 64);//temp
					RenderTextureFromSheet(gRenderer, itemSheet, invArray[invPos][0], invItemRect);
				}else{
					invArray[invPos][0] = -1;
					invArray[invPos][1] = 0;
				}
				// printf("%d ", invArray[y][x][0]);
			}
			// printf("\n");
		}
		if(mouseInv[0] > -1 && mouseInv[1] > 0){
			SDL_Rect mouseItem = {mousePos.x - 16, mousePos.y - 16, 32, 32};
			
			// SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 128);	//temp
			// SDL_RenderDrawRect(gRenderer, &mouseItem);			//temp
			// SDL_SetRenderDrawColor(gRenderer, 200, 200, 200, 64);//temp
			RenderTextureFromSheet(gRenderer, itemSheet, mouseInv[0], mouseItem);
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
			invArray[cell + 1][1] = itemQty;
			
		}else if(strcmp(mode, "add") == 0){
			if(invArray[cell + 1][1] < 99){
				invArray[cell + 1][1] += itemQty;
			}
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
			if((invArray[invPos][0] == type && invArray[invPos][1] <= 99) || (invArray[invPos][0] == -1)){
				// if(invArray[y][x][1] == amount){
					return invPos;
				// }
			}
		}
	}
}