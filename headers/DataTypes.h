#ifndef DATATYPE_H_
# define DATATYPE_H_

typedef struct{
	int x;
	int y;
}Vector2;

typedef struct{
	float x;
	float y;
}fVector2;


//INV Structs
typedef struct{
	SDL_Texture *tex;
	int w;
	int h;
	int tileW;
}WB_Tilesheet;

typedef struct{
	Vector2 pos;
	int itemType;
	int itemQty;
}GroundItem;

typedef struct{
	char name[64];
	char description[128];
	
}INV_Item;

typedef struct{
	int 
	
}INV_Recipe;

#endif