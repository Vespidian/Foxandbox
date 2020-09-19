--[=[

Pimy: 2020
Basic testing of the lua implementation to interact with game

]=]
register_tilesheet({
	name = "blocks",
	path = "images/blocks.png",
	tile_size = 16,
})

register_tilesheet({
	name = "furniture",
	path = "images/furniture.png",
	tile_size = 16,
})

register_tilesheet({
	name = "character",
	path = "images/foxanim.png",
	tile_size = 16,
})

register_tilesheet({
	name = "items",
	path = "images/items.png",
	tile_size = 16,
})

register_tilesheet({
	name = "ui",
	path = "images/ui.png",
	tile_size = 16,
})


register_item({
	name = "stone",
	sheet = "items",
	tile_index = 1,
})
register_item({
	name = "wood",
	sheet = "items",
	tile_index = 2,
})
register_item({
	name = "feather",
	sheet = "items",
	tile_index = 4,
})

register_block({
	name = "air",
	item_sheet = "blocks",
	item_tile_index = 0,

	block_sheet = "blocks",
	block_tile_index = 0,

	block_layer = "feature",
	collision_type = -1,
})
register_block({
	name = "bookshelf",
	item_sheet = "furniture",
	item_tile_index = 0,
	
	block_sheet = "furniture",
	block_tile_index = 0,
	
	block_layer = "feature",
	collision_type = 0,
})

register_block({
	name = "flower",
	item_sheet = "furniture",
	item_tile_index = 1,
	
	block_sheet = "furniture",
	block_tile_index = 1,
	
	block_layer = "feature",
	collision_type = 1,
})


register_block({
	name = "grass",
	item_sheet = "blocks",
	item_tile_index = 1,

	block_sheet = "blocks",
	block_tile_index = 1,

	collision_type = -1,
	flags = {"ground"},
})

register_block({
	name = "water",
	item_sheet = "blocks",
	item_tile_index = 2,

	block_sheet = "blocks",
	block_tile_index = 2,
	block_layer = "terrain",

	collision_type = 0,
})

register_block({
	name = "nylium",
	item_sheet = "blocks",
	item_tile_index = 3,

	block_sheet = "blocks",
	block_tile_index = 3,

	collision_type = -1,
})

register_block({
	name = "not_gate",
	item_sheet = "blocks",
	item_tile_index = 8,

	block_sheet = "blocks",
	block_tile_index = 8,

	block_layer = "feature",
	enable_rotation = true,

	collision_type = -1,
})


inventory_add("not_gate", 99)
inventory_add("not_gate", 99)
inventory_add("not_gate", 99)

inventory_add("nylium", 32)
-- populate_autotile({
-- 	name = "grass_water",
-- 	base_block = "grass",
-- 	sub_block = "water",
-- })

-- populate_autotile({
-- 	name = "nylium_water",
-- 	base_block = "nylium",
-- 	sub_block = "water",
-- })
-- register_block({
-- 	name = "woopsy",
-- })
-- register_tile()