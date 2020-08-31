-- register_tile("base:null", {
	-- "huh", 
	-- "no", 
	-- "oh god why"
-- }, {})

-- register_tile("base:grass", {
	-- "heyo",
	-- "ndso",
	-- "bflag"
-- }, {
	-- tile = 567
-- })

register_tilesheet({
	name = "default_ground",
	path = "images/AUTOTILE_grass-water.png",
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

register_block({name = "null"})

register_block({
	name = "bookshelf",
	item_sheet = "furniture",
	item_tile_index = 0,
	
	block_sheet = "furniture",
	block_tile_index = 0,
	
	block_layer = "feature",
	collision_type = 0,
	-- flags = {"feature", "collision_0"},
})

register_block({
	name = "air",
	item_sheet = "furniture",
	item_tile_index = 7,

	block_sheet = "furniture",
	block_tile_index = 7,

	block_layer = "feature",
	collision_type = -1,
})
register_block({
	name = "flower",
	item_sheet = "furniture",
	item_tile_index = 1,

	block_sheet = "furniture",
	block_tile_index = 1,

	block_layer = "feature",
	collision_type = 1,

	-- flags = {"feature"},
})


register_block({
	name = "grass",
	item_sheet = "default_ground",
	item_tile_index = 0,

	block_sheet = "default_ground",
	block_tile_index = 0,

	collision_type = -1,
	flags = {"ground"},
})

register_block({
	name = "water",
	item_sheet = "default_ground",
	item_tile_index = 47,

	block_sheet = "default_ground",
	block_tile_index = 47,
	block_layer = "terrain",
	-- block_type = "feature",

	collision_type = 0,
	-- flags = {"ground", "collision_0"},
})

register_block({
	name = "nylium",
	item_sheet = "furniture",
	item_tile_index = 4,

	block_sheet = "furniture",
	block_tile_index = 4,

	collision_type = -1,
	-- flags = {"ground", "collision_0"},
})



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