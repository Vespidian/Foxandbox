#include "../global.h"
#include "../renderer/tilesheet.h"
#include "../renderer/renderer.h"

//UI Elements
#include "resizable_rect.h"

#include "ui.h"

void InitUI(){
	NewRawTilesheet("ui", "../images/ui/ui.png", (Vector2){16, 16});

	InitResizableRect();

	DebugLog(D_ACT, "Initialized UI subsystem");
}

SDL_Rect VerticalRectList(int num_items, int itemIndex, Vector2 size, Vector2 origin, int spacing){
	SDL_Rect rect = {0, 0, size.x, size.y};
	int yOffset = 0;
	if(num_items % 2 == 0){//Even number of elements
		rect.x = origin.x - size.x / 2;
		if(itemIndex < num_items / 2){//Top half
			yOffset = (num_items / 2 - itemIndex);
			rect.y = origin.y - (spacing / 2 + size.y * yOffset + spacing * (yOffset - 1));
		}else{//Bottom half
			yOffset = (itemIndex - num_items / 2);
			rect.y = origin.y + (spacing / 2 + size.y * yOffset + spacing * yOffset);
		}
	}else{//Odd number of elements
		rect.x = origin.x - size.x / 2;
		if(itemIndex < num_items / 2){//top half
			yOffset = (num_items / 2 - itemIndex);
			rect.y = origin.y - (size.y / 2 + size.y * yOffset + spacing * yOffset);
		}else if(itemIndex == (num_items + 1) / 2 - 1){//Middle element
			rect.y = origin.y - (size.y / 2);
		}else{//Bottom half
			yOffset = (itemIndex - 1 - num_items / 2);
			rect.y = origin.y + (size.y / 2 + size.y * yOffset + spacing * (yOffset	+ 1));
		}
	}
	return rect;
}

void DrawVListBackground(int num_items, Vector2 size, Vector2 origin, int spacing, uint8_t opacity){
	int padding = 8;
	SDL_Rect rect;
	rect.w = size.x + padding * 2;
	rect.h = num_items * size.y + (num_items + 1) * padding;
	rect.x = SCREEN_WIDTH / 2 - rect.w / 2;
	rect.y = SCREEN_HEIGHT / 2 - rect.h / 2;
	PushRender_Tilesheet(renderer, FindTilesheet("ui"), 0, rect, RNDR_UI);
}