#ifndef ENTITIES_H_
#define ENTITIES_H_


typedef struct{
	char *name;
	SDL_Texture *tex;
	int tile_size;
	int w;
	int h;
}TilesheetComponent;

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
	
	TilesheetComponent *pSheet;
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
	TilesheetComponent sheet;
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

	TilesheetComponent sheet;//Block tilesheet
	int tile;//Block tile index

	bool allowRotation;

	int collisionType;
	char layer[16];

}BlockComponent;

typedef struct {
	int inItem;
	int inQty;
	int outItem;
	int outQty;
	
}RecipeComponent;

//Render Components
typedef struct{
	SDL_Renderer *renderer;
	TilesheetComponent *tileSheet;
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