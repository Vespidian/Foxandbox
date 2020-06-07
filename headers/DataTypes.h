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
/* typedef struct{
	Vector2 pos;
	int itemType;
	int itemQty;
}GroundItem; */

typedef struct{
	char name[64];
	char description[128];
	WB_Tilesheet sheet;
	int tile;
	
}INV_Item;

typedef struct{
	INV_Item item;
	INV_Item itemDrop;
	int dropQty;
	
}INV_Block;

typedef struct{
	int inItem;
	int inQty;
	int outItem;
	int outQty;
	
}INV_Recipe;

#endif