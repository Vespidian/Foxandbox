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

register_block({
	name = "bookshelf",
	item_sheet = "images/furniture.png",
	item_tile_index = 0,
	
	block_sheet = "images/furniture.png",
	block_tile_index = 0,
	-- flags = {"bflag_default", "bflag_notfault", "cflag whoah?", "wait it works holy crap!"},
})

register_block({
	name = "flower",
	item_sheet = "images/furniture.png",
	item_tile_index = 1,

	block_sheet = "images/furniture.png",
	block_tile_index = 1,
	-- flags = {"bflag_default", "bflag_notfault", "cflag whoah?", "wait it works holy crap!"},
})
register_block({
	name = "grass",
	item_sheet = "images/AUTOTile_grass-water.png",
	item_tile_index = 0,

	block_sheet = "images/AUTOTile_grass-water.png",
	block_tile_index = 0,
	-- flags = {"bflag_default", "bflag_notfault", "cflag whoah?", "wait it works holy crap!"},
})

register_block({
	name = "water",
	item_sheet = "images/AUTOTile_grass-water.png",
	item_tile_index = 47,

	block_sheet = "images/AUTOTile_grass-water.png",
	block_tile_index = 47,
	-- flags = {"bflag_default", "bflag_notfault", "cflag whoah?", "wait it works holy crap!"},
})




populate_autotile({
	name = "grass_water",
	base_block = "grass",
	sub_block = "water",
})

-- register_block({
-- 	name = "woopsy",
-- })
-- register_tile()