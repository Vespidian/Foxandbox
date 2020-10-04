#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <luaconf.h>

#include "headers/DataTypes.h"
#include "headers/ECS.h"
#include "headers/initialize.h"
#include "headers/inventory.h"
#include "headers/lua_systems.h"

TilesheetComponent *tilesheets = NULL;
int num_tilesheets = 0;

BlockComponent undefinedBlock;
BlockComponent *blockData;
int numBlocks = -1;

ItemComponent undefinedItem;
ItemComponent *itemData;
int numItems = -1;



int runScript(char *fileName){
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	luaL_dofile(L, fileName);
	lua_close(L);
	return 0;
}

void loadLua(){
	DebugLog(D_ACT, "Initializing LUA -----------------------");
	num_tilesheets = 0;
	numItems = -1;
	numBlocks = -1;

	tilesheets = malloc(sizeof(TilesheetComponent));
	itemData = malloc(sizeof(ItemComponent));
	blockData = malloc(sizeof(BlockComponent));
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	
	lua_register(L, "register_item", register_item);
	lua_register(L, "register_tilesheet", register_tilesheet);
	lua_register(L, "register_block", register_block);
	lua_register(L, "inventory_add", inventory_add);
	
	if(luaL_dofile(L, "scripts/init.lua")){	
        DebugLog(D_SCRIPT_ERR, lua_tostring(L, -1));
        printf("%s\n", lua_tostring(L, -1));
    }
	lua_close(L);
	DebugLog(D_ACT, "LUA initialization done ----------------");
}

TilesheetComponent *find_tilesheet(char *name){
	for(int i = 0; i < num_tilesheets; i++){
		if(strcmp(name, tilesheets[i].name) == 0){
			return &tilesheets[i];
		}
	}
	return &undefinedSheet;
}

ItemComponent *find_item(char *name){
	for(int i = 0; i < numItems + 1; i++){
		if(strcmp(name, itemData[i].name) == 0){
			return &itemData[i];
		}
	}
	return &undefinedItem;
}

BlockComponent *find_block(char *name){
	for(int i = 0; i < numBlocks + 1; i++){
		if(strcmp(name, blockData[i].item->name) == 0){
			return &blockData[i];
		}
	}
	return &undefinedBlock;
}

int register_tilesheet(lua_State *L){
	tilesheets = realloc(tilesheets, (num_tilesheets + 1) * sizeof(TilesheetComponent));
	
	luaL_checktype(L, 1, LUA_TTABLE);
	lua_getfield(L, -1, "name");
	if(lua_tostring(L, -1) != NULL){
        tilesheets[num_tilesheets].name = malloc(strlen(lua_tostring(L, -1) + 1) * sizeof(char));
		strcpy(tilesheets[num_tilesheets].name, lua_tostring(L, -1));
	}else{
        tilesheets[num_tilesheets].name = malloc((strlen("undefined") + 1) * sizeof(char));
		strcpy(tilesheets[num_tilesheets].name, "undefined");
	}
	
	lua_getfield(L, -2, "path");
	if(lua_tostring(L, -1) != NULL){
		tilesheets[num_tilesheets].tex = IMG_LoadTexture(renderer, lua_tostring(L, -1));
	}else{
		tilesheets[num_tilesheets].tex = IMG_LoadTexture(renderer, "undefined");
	}

	lua_getfield(L, -3, "tile_size");
	if(lua_tonumber(L, -1)){
		tilesheets[num_tilesheets].tile_size = lua_tonumber(L, -1);
		//Identify the width and height of the tilesheet based on tile size
		SDL_QueryTexture(tilesheets[num_tilesheets].tex, NULL, NULL, &tilesheets[num_tilesheets].w, &tilesheets[num_tilesheets].h);
		tilesheets[num_tilesheets].w /= tilesheets[num_tilesheets].tile_size;
		tilesheets[num_tilesheets].h /= tilesheets[num_tilesheets].tile_size;
	}

	DebugLog(D_SCRIPT_ACT, "Loaded tilesheet '%s'", tilesheets[num_tilesheets].name);

	num_tilesheets++;
	return 0;
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


	DebugLog(D_SCRIPT_ACT, "Created item '%s'", itemData[numItems].name);
	return 0;
}

int register_block(lua_State *L){
	numBlocks++;
	blockData = realloc(blockData, (numBlocks + 10) * sizeof(BlockComponent));

	luaL_checktype(L, 1, LUA_TTABLE);

	lua_getfield(L, -1, "name");
	// if(strcmp(find_item((char *)lua_tostring(L, -1))->name, "undefined") == 0){//Check if the item already exists
	if(&find_item((char *)lua_tostring(L, -1))->name == &undefinedItem.name){//Check if the item already exists
		numItems++;
		itemData = realloc(itemData, (numItems + 1) * sizeof(ItemComponent));

		if(lua_tostring(L, -1) != NULL && strlen(lua_tostring(L, -1)) > 0){
			itemData[numItems].name = malloc(sizeof(char) * (strlen(lua_tostring(L, -1)) + 1));
			strcpy(itemData[numItems].name, lua_tostring(L, -1));
		}else{
			itemData[numItems].name = malloc(sizeof(char) * (strlen("undefined") + 1));
			strcpy(itemData[numItems].name, "undefined");
		}

		lua_getfield(L, -2, "item_sheet");
		if(lua_tostring(L, -1) != NULL){
			itemData[numItems].sheet = *find_tilesheet((char *)lua_tostring(L, -1));
		}else{
			itemData[numItems].sheet = undefinedSheet;
		}

		lua_getfield(L, -3, "item_tile_index");
		if(lua_tonumber(L, -1)){
			itemData[numItems].tile = lua_tonumber(L, -1);
		}else{
			itemData[numItems].tile = 0;
		}

		blockData[numBlocks].item = &itemData[numItems];
		itemData[numItems].isBlock = true;

		DebugLog(D_SCRIPT_ACT, "Created item '%s'", itemData[numItems].name);
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
	if(lua_tonumber(L, -1) || lua_tonumber(L, -1) == 0){
		blockData[numBlocks].tile = lua_tonumber(L, -1);
	}else{
		blockData[numBlocks].tile = -1;
	}

	lua_getfield(L, -6, "dropped_item");
	if(lua_tostring(L, -1) != NULL){
		if(find_item((char *)lua_tostring(L, -1)) != NULL){
			blockData[numBlocks].dropItem = find_item((char *)lua_tostring(L, -1));
		}else{
			blockData[numBlocks].dropItem = blockData[numBlocks].item;
		}
	}else{
		blockData[numBlocks].dropItem = blockData[numBlocks].item;
	}

	lua_getfield(L, -7, "dropped_qty");
	if(lua_tonumber(L, -1)){
		blockData[numBlocks].dropQty = lua_tonumber(L, -1);
	}else{
		blockData[numBlocks].dropQty = 1;
	}

	// lua_pop(L, 1);
	lua_getfield(L, -8, "collision_type");
	if(lua_toboolean(L, -1)){
		blockData[numBlocks].collisionType = lua_tonumber(L, -1);
	}else{
		blockData[numBlocks].collisionType = -1;
	}

	lua_getfield(L, -9, "block_layer");
	if(lua_tostring(L, -1)){
		strcpy(blockData[numBlocks].layer, lua_tostring(L, -1));
	}else{
		strcpy(blockData[numBlocks].layer, "terrain");
	}

	if(strcmp(blockData[numBlocks].layer, "terrain") == 0 && blockData[numBlocks].collisionType > 0){
		blockData[numBlocks].collisionType = -1;
	}

	lua_getfield(L, -10, "enable_rotation");
	if(lua_toboolean(L, -1)){
		blockData[numBlocks].allowRotation = lua_toboolean(L, -1);
	}else{
		blockData[numBlocks].allowRotation = false;
	}

	DebugLog(D_SCRIPT_ACT, "Created block '%s'", blockData[numBlocks].item->name);

	return 0;
}

int inventory_add(lua_State *L){
	int qty = 0;
	if(lua_tonumber(L, 2)){
		qty = lua_tonumber(L, 2);
	}
	if(lua_tostring(L, 1)){
		INV_Add(qty, find_item((char *)lua_tostring(L, 1)));
	}
	return 0;
}