#ifndef DATATYPE_H_
# define DATATYPE_H_

typedef struct{
	int x;
	int y;
}Vector2;

typedef struct{
	int min;
	int max;
}Range;

//INV Structs
typedef struct{
	SDL_Texture *tex;
	int w;
	int h;
	int tileW;
}WB_Tilesheet;


//Inventory items

#endif