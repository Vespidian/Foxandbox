#ifndef LUA_SYSTEMS_H_
#define LUA_SYSTEMS_H_

/** 
 *  \file 
 *  \brief Definition of all lua function wrappers and dynamic arrays
 */

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <luaconf.h>

/**
 *  Quickly run a lua script via path to script
 */
int runScript(char *fileName);

/**
 *  Run at startup, loads base game elements
 *  Can be rerun to reload base game
 */
void loadLua();


/**
 *  Current number of loaded tilesheets
 */
extern int num_tilesheets;

/**
 *  Dynamic array of all tilesheets
 */
extern TilesheetComponent *tilesheets;

/**
 *  Lua function wrapper to add element to tilesheets array with various parameters and paths
 */
int register_tilesheet(lua_State *L);

/**
 *  Iterate through tilesheets array to find specified tilesheet name
 * 	\return A pointer to the found item or to undefinedSheet if it cannot be found
 */
TilesheetComponent *find_tilesheet(char *name);


/**
 *  Current number of loaded items
 */
extern int numItems;

/**
 *  Null item to set when item is missing members
 */
extern ItemComponent undefinedItem;

/**
 *  Dynamic array of all items
 */
extern ItemComponent *itemData;

/**
 *  Lua function wrapper to add element to itemData array with various parameters
 */
int register_item(lua_State *L);

/**
 *  Iterate through itemData array to find specified item name
 * 	\return A pointer to the found item or to undefinedItem if it cannot be found
 */
ItemComponent *find_item(char *name);


/**
 *  Current number of loaded blocks
 */
extern int numBlocks;

/**
 *  Null block to set when block is missing members
 */
extern BlockComponent undefinedBlock;

/**
 *  Dynamic array of all blocks
 */
extern BlockComponent *blockData;

/**
 *  Lua function wrapper to add element to blockData array with various parameters
 */
int register_block(lua_State *L);

/**
 *  Iterate through blockData array to find specified block
 *  \return A pointer to the found block or to undefinedBlock if it cannot be found
 */
BlockComponent *find_block(char *name);


/**
 *  Lua function wrapper to add an item to the player inventory
 */
int inventory_add(lua_State *L);

#endif