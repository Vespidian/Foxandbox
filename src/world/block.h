#ifndef BLOCK_H_
#define BLOCK_H_

#include "../entities/item.h"

/**
 *  Block struct containing all data needed to define behaviour of a block
 */
typedef struct BlockObject{
    unsigned int id;
    ItemObject item;
    ItemObject break_item;
    TilesheetObject tilesheet;
    int tile_index;
    bool allow_rotation;
    bool allow_autotile;
}BlockObject;

/**
 *  Block to be used in case of no block
 */
extern BlockObject air_block;

/**
 *  Block to be used in error cases
 */
extern BlockObject undefined_block;

/**
 *  Array containing all defined blocks
 */
extern BlockObject *block_stack;

/**
 *  Initialize the block subsystem
 */
void InitBlocks();

/**
 *  @brief Create a new block with specified parameters and append it to the block stack
 *  @param name the name of the item the block is based upon (only used when 'item' is null)
 *  @param item the item for the block to be based upon (null for new item)
 *  @param break_item the item to be dropped when block is broken (null to use 'BlockObject.item')
 *  @param tilesheet block texture tilesheet
 *  @param tile_index index of the block's texture
 *  @param allow_rotation whether or not the block can be oriented in multiple directions
 *  @param allow_autotile whether or not the block can be merged with surrounding autotile blocks (for ground / terrain blocks)
 *  @return A pointer to the newly created block in the block stack
 */
BlockObject *NewBlock(const char *name, ItemObject *item, ItemObject *break_item, TilesheetObject *tilesheet, int tileIndex, bool allow_rotation, bool allow_autotile);

/**
 *  @return Block with name 'name' from block stack
 */
BlockObject *FindBlock(const char *name);

/**
 *  @return Block with id 'id' from block stack
 */
BlockObject *FindBlock_id(unsigned int id);

#endif