#include "../global.h"
#include "animation.h"
#include "../render/tilesheet.h"

enum AnimType{ANIM_FRAME};

long IntegerLerp(long start, long end, uint16_t amount){
	long lerpLimit = 1;
	long velocity = (end - start) * ((float)amount / 65535.0f);
	if(velocity < lerpLimit && start + lerpLimit <= end){
		return lerpLimit;
	}
	return velocity;
}

// void AnimateStretch(SDL_Rect *destination, int stretchFactor, int duration){
	//Add animator to animator array with specified properties (duration left, stretch factor, current stretch, destination rect)

// }

// void Anim_SlideIn(SDL_Rect *src, )

void NewAnimation_tilesheet(TilesheetObject *tilesheet, SDL_Rect dest, int startFrame, int numFrames, int delay){

}