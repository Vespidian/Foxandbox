#ifndef ITEM_H_
#define ITEM_H_

#include "../renderer/tilesheet.h"

/**
 *  Item struct containing the data of a single item
 */
typedef struct ItemObject{
    const char *name;
    unsigned int id;
    TilesheetObject tilesheet;
    unsigned int tile_index;
    bool isBlock;
}ItemObject;

/**
 *  Array of all items
 */
extern ItemObject *item_stack;

/**
 *  Item to be used in error cases
 */
extern ItemObject undefined_item;

/**
 *  @brief Initialize the item subsystem
 */
void InitItems();

/**
 *  @brief Create a new item with specified parameters and append it to the item stack
 *  @param name item's name
 *  @param tilesheet tilesheet to be used by the item
 *  @param tile_index item texture's tilesheet index
 *  @return A pointer to the item in the item stack
 */
ItemObject *NewItem(const char *name, TilesheetObject *tilesheet, int tile_index);

/**
 *  @return Item with name 'name' from item stack
 */
ItemObject *FindItem(const char *name);

/**
 *  @return Item with id 'id' from item stack
 */
ItemObject *FindItem_id(unsigned int id);

#endif