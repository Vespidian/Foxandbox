#ifndef ENTITIES_H_
#define ENTITIES_H_




//Particle System
typedef struct{
	bool active;
	SDL_Rect pos;
	// int type;
	int size;
	int initDuration;
	int duration;
	Vector2 dir;
	// SDL_Texture *pTex;
	
}ParticleComponent;

typedef struct{
	ParticleComponent *particles;
	int pType;
	int maxParticles;
	Range duration;
	bool fade;
	
	bool playSystem;
	SDL_Rect area;
	bool boundaryCheck;
	Range xR;
	Range yR;
	
	WB_Tilesheet *pSheet;
	SDL_Rect destRect;
	
}ParticleSystem;


//Entity Components
typedef struct{
	int speed;
	Vector2 velocity;
	// bool followPlayer;
}MovementComponent;

typedef struct{
	Vector2 tilePos;
	Vector2 screenPos;
	Vector2 worldPos;
}TransformComponent;

typedef struct{
	SDL_Rect collisionBox;
	SDL_Rect boundingBox;
	
	bool noClip;
	
	bool colUp;
	bool colDown;
	bool colLeft;
	bool colRight;
}CollisionComponent;

typedef struct{
	int health;
	int maxHealth;
}HealthComponent;

typedef struct{
	bool hostile;
	int damage;
}AttackComponent;


//Inventory components
typedef struct{
	char *name;
	// char name[64];
	WB_Tilesheet sheet;
	int tile;
	bool isBlock;
	
}ItemComponent;

typedef struct{
	ItemComponent *item;
	int qty;
	TransformComponent transform;
	float animLocation;
	int animDir;
}DroppedItemComponent;

typedef struct{
	ItemComponent *item;//Base item
	ItemComponent *dropItem;//Item to drop when block broken, if null just drop item
	int dropQty;//Number of items to drop

	WB_Tilesheet sheet;//Block tilesheet
	int tile;//Block tile index

	bool autoTile;//Is this block from an autotile set?

	int collisionType;
	char layer[16];

	char **flags;
	
}BlockComponent;

typedef struct{
	ItemComponent item;
	ItemComponent dropItem;//If null just drop item
	int dropQty;//If null drop 1

	WB_Tilesheet sheet;
	int tile;

	bool rotate;
	bool autoTile;

	char **flags;
	
}BlockComponent_local;

typedef struct{
	char *name;
	BlockComponent *baseBlock;
	BlockComponent *auto_block;//Array
	BlockComponent *subBlock;
}AutotileComponent;

typedef struct {
	int inItem;
	int inQty;
	int outItem;
	int outQty;
	
}RecipeComponent;

//Render Components
typedef struct{
	SDL_Renderer *renderer;
	WB_Tilesheet *tileSheet;
	int tile;
	SDL_Rect transform;
	int alpha;
	int zPos;
	int rotation;
	SDL_Color colorMod;
}RenderComponent;

typedef struct{
	int type;
	int zPos;
	int rotation;
	BlockComponent *block;
}RenderTileComponent;



// typedef struct{
// 	char name[128];
// 	Vector2 size;
// 	RenderTileComponent terrain[32][32];
// 	RenderTileComponent features[32][32];
// 	int collision[32][32];
// 	Vector2 *spawns;
// }LevelComponent_old;

typedef struct{
	char *name;
	unsigned long seed;
	Vector2 map_size;
	RenderTileComponent **terrain;
	RenderTileComponent **features;
	int **collision;
	Vector2 *spawns;
}LevelComponent;


//Entity Types
typedef struct{
	int id;
	
	RenderComponent renderer;
	TransformComponent transform;
	CollisionComponent collider;
	
	MovementComponent movement;
	HealthComponent health;
	AttackComponent attack;
	
}Entity;

extern Entity character;

#endif