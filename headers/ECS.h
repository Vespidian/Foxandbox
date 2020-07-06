#ifndef ENTITIES_H_
#define ENTITIES_H_

//Render Components
typedef struct{
	SDL_Renderer *renderer;
	WB_Tilesheet tileSheet;
	int tile;
	SDL_Rect transform;
	int alpha;
	int zPos;
}RenderComponent;

typedef struct{
	int type;
	int zPos;
}RenderTileComponent;



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
	
	WB_Tilesheet pSheet;
	SDL_Rect destRect;
	
}ParticleSystem;



//Inventory components
typedef struct{
	char name[64];
	char description[128];
	WB_Tilesheet sheet;
	int tile;
	
}ItemComponent;

typedef struct{
	ItemComponent item;
	ItemComponent itemDrop;
	char **flags;
	int dropQty;
	
}BlockComponent;

typedef struct{
	int inItem;
	int inQty;
	int outItem;
	int outQty;
	
}RecipeComponent;



//Entity Components
typedef struct{
	int speed;
	bool followPlayer;
}MovementComponent;

typedef struct{
	Vector2 tilePos;
	Vector2 screenPos;
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