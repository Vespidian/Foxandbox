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