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

register_block({
	name = "block",
	item_sheet = "image.png",
	item_tile_index = 0,

	block_sheet = "image.png",
	block_tile_index = 0,
	flags = {"bflag_default", "bflag_notfault", "cflag whoah?", "wait it works holy crap!"},
})

-- register_block({
-- 	name = "woopsy",
-- })
-- register_tile()