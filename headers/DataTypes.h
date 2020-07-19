#ifndef DATATYPE_H_
# define DATATYPE_H_

typedef struct{
	int x;
	int y;
}Vector2;

typedef struct{
	double x;
	double y;
}fVector2;

typedef struct{
	int min;
	int max;
}Range;

//INV Structs
typedef struct{
	char name[64];
	SDL_Texture *tex;
	int tile_size;
	int w;
	int h;
}WB_Tilesheet;


//Inventory items

#endif