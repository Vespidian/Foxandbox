#include "../global.h"
#include "renderer.h"
#include "../world/chunk.h"
#include "../world/sandbox_generation.h"

const int autotile_correlations[46] = {
    2, 8, 10, 11, 16, 18, 22, 24, 26, 27, 30, 31, 64, 66, 72, 74, 75, 
    80, 82, 86, 88, 90, 91, 94, 95, 104, 106, 107, 120, 122, 123, 126, 
    127, 208, 210, 214, 216, 218, 219, 222, 223, 248, 250, 251, 254
};

Vector2 surround_correlation[8] = {
    {-1, -1}, {0, -1}, {1, -1},
    {-1,  0},          {1,  0},
    {-1,  1}, {0,  1}, {1,  1}
};

void InitAutotile(){
    // NewTilesheetFromFile("autotile_mask", "../images/autotile_mask.png", (Vector2){16, 16});
}

/**
 *  When a tile is changed, the surround 8 tiles need their autotiles recalculated
 */
void ChooseSubBase(Vector2 position){
    /*Vector2 tmp_chunk;
    Vector2 tmp_offset;
    int base_id;
    int sub_id;

    CoordinateConvert(position, &tmp_chunk, &tmp_offset);
    base_id = FindChunk(chunk)->tile[0][position.y][position.x].block;

    // Iterate through all directions to find background tile
    for(int i = 0; i < 8; i++){
        if((base_id->autotile_value_uncompressed >> i) & 1){
            sub_id = FindChunk(chunk)->tile[0][surround_correlation[i].y][surround_correlation[i].x].block;
        }
    }*/
}

void RenderTerrainAutotile(SDL_Renderer *renderer, Vector2 position, SDL_Rect dst, int zPos){
    // Vector2 tmp_chunk;
    // Vector2 tmp_offset;

    // TileObject base_id;
    // TileObject sub_id;

    // CoordinateConvert(position, &tmp_chunk, &tmp_offset);
    // base_id = FindChunk(tmp_chunk)->tile[0][tmp_offset.y][tmp_offset.x];
    // // Iterate through all directions to find background tile
    // for(int i = 0; i < 8; i++){
    //     if((FindChunk(tmp_chunk)->tile[0][tmp_offset.y][tmp_offset.x].autotile_value_uncompressed >> i) & 1){
    //         // Use surround correlation to offset tile in specified direction
    //         CoordinateConvert((Vector2){position.x + surround_correlation[i].x, position.y + surround_correlation[i].y}, &tmp_chunk, &tmp_offset);
    //         sub_id = FindChunk(tmp_chunk)->tile[0][tmp_offset.y][tmp_offset.x];
    //         break;
    //     }
    // }
    // // CoordinateConvert(position, &tmp_chunk, &tmp_offset);

    // // Retrieve tilesize of base block tilesheet
    // int tmp_tile_size = IDFindTilesheet(FindBlock_id(base_id.block)->tilesheet)->tileSize.x;
    // // Create texture we will be incrementally building on
    // SDL_Texture *output_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, tmp_tile_size, tmp_tile_size);//
    // //Create a TextureObject for adding to renderqueue    
    // // TextureObject tex_object = {"autotile", 0, output_texture, tmp_tile_size, tmp_tile_size};
    

    // //Start image processing
    // SDL_SetRenderTarget(renderer, output_texture);
    // SDL_RenderClear(renderer);

    // //tmp
    // // SDL_RenderCopy(renderer, IDFindTilesheet(FindBlock_id(sub_id.block)->tilesheet)->texture, NULL, NULL);
    // SDL_RenderCopy(renderer, IDFindTexture(IDFindTilesheet(FindBlock_id(sub_id.block)->tilesheet)->texture)->texture, NULL, NULL);
    // SDL_SetRenderTarget(renderer, NULL);

    /**
     *  Render the base block
     *  Render the autotile_mask on top with blend mode add
     *  
     */

}

uint8_t CalculateAutotile(Vector2 position){
    // Vector2 tmp_chunk;
    // Vector2 tmp_offset;

    // CoordinateConvert(position, &tmp_chunk, &tmp_offset);
    // int base_tile_id = FindChunk(tmp_chunk)->tile[0][tmp_offset.y][tmp_offset.x].block;

    // int autotile_value = 0;
    // int autotile_value_uncompressed = 0;
    // bool tiles[8];
    // /* Corresponding 'tiles' array indices
	//  0  |  1  | 2
	// ____|_____|____
	//  3  | /// | 4
	// ____|_____|____
	//  5  |  6  | 7
	// */

    // for(int y = position.y - 1; y <= position.y + 1; y++){
    //     for(int x = position.x - 1; x <= position.x + 1; x++){
    //         if(y != position.y || x != position.x){
    //             CoordinateConvert((Vector2){x, y}, &tmp_chunk, &tmp_offset);
    //             tiles[y * 3 + x] = FindChunk(tmp_chunk)->tile[0][tmp_offset.y][tmp_offset.x].block != base_tile_id;
    //         }
    //     }
    // }
    // autotile_value_uncompressed = autotile_value;

    // // Reduce possible options down to 47
    // if(tiles[0]){
    //     if(!tiles[1] || !tiles[3]){
    //         tiles[0] = false;
    //     }
    // }
    // if(tiles[2]){
    //     if(!tiles[1] || !tiles[4]){
    //         tiles[2] = false;
    //     }
    // }
    // if(tiles[5]){
    //     if(!tiles[6] || !tiles[3]){
    //         tiles[5] = false;
    //     }
    // }
    // if(tiles[7]){
    //     if(!tiles[6] || !tiles[4]){
    //         tiles[7] = false;
    //     }
    // }

    // // Accumulate the tiles into the final autotile_value
    // for(int i = 0; i < 8; i++){
    //     autotile_value += tiles[i] << i;
    // }

    // // Filter the 47 possible options using the correlation array
    // if(autotile_value != 0 && autotile_value != 255){
    //     for(int i = 0; i < 47; i++){
    //         if(autotile_value == autotile_correlations[i - 1]){
    //             autotile_value = i;
    //             break;
    //         }
    //     }
    // }
    // return autotile_value + (autotile_value_uncompressed << 8);
	return 0;
}