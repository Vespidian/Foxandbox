#ifndef ENTITIES_H_
#define ENTITIES_H_

//TEMP
void RenderEntities();
void NewEntity();

//COMPONENTS
typedef struct{
	SDL_Renderer *renderer;
	WB_Tilesheet tileSheet;
	int tile;
	SDL_Rect transform;
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
	int type;
	int size;
	int initDuration;
	int duration;
	Vector2 dir;
	SDL_Texture *pTex;
	
}ParticleComponent;

typedef struct{
	ParticleComponent *particles;
	int pType;
	int maxParticles;
	Range duration;
	bool fade;
	
	bool playSystem;
	SDL_Rect area;
	Range xR;
	Range yR;
	
	WB_Tilesheet pSheet;
	SDL_Rect destRect;
	
}ParticleSystem;


//Mob Components
typedef struct{
	int speed;
	bool followPlayer;
}FollowComponent;

typedef struct{
	int health;
	int maxHealth;
}HealthComponent;

typedef struct{
	bool hostile;
	int damage;
}AttackComponent;



//ENTITIES
typedef struct{
	int id;
	RenderComponent renderer;
	FollowComponent follow;
	HealthComponent health;
	AttackComponent attack;
	
}Entity;



#endif