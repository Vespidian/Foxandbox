#ifndef ANIMATION_H_
#define ANIMATION_H_

typedef struct AnimatorObject{
	unsigned int id;
	unsigned int tilesheet;
	
}AnimatorObject;

long IntegerLerp(long start, long end, uint16_t amount);
#endif