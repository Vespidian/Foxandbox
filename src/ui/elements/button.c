#include "../../global.h"
#include "../../renderer/renderer.h"
#include "../../renderer/render_text.h"
#include "../../event.h"
#include "../resizable_rect.h"

#include "button.h"

bool Button(SDL_Renderer *renderer, SDL_Rect rect, char *text){
	bool isClicked = false;

	SDL_Point mouse;
	SDL_GetMouseState(&mouse.x, &mouse.y);

	if(SDL_PointInRect(&mouse, &rect)){
		rect.x -= 2;
		rect.y -= 2;
		rect.w += 4;
		rect.h += 4;
		if(mouse_held){
			rect.x += 1;
			rect.y += 1;
			rect.w -= 3;
			rect.h -= 3;
		}
		if(mouse_clicked){
			isClicked = true;
		}
	}

	ResizableRect(rect, 10);

	Vector2 textPos = {(rect.x + rect.w / 2) - (strlen(text) * 10) / 2, (rect.y + rect.h / 2) - 8};
	RenderText(renderer, FindFont("default_font"), 1, textPos.x, textPos.y, text);

	return isClicked;
}

bool Button_format(SDL_Renderer *renderer, SDL_Rect rect, const char *text, ...){
	va_list vaFormat;
	
	//Use var args to create formatted text
	va_start(vaFormat, text);
	char *formattedText = malloc((strlen(text) + 64) * sizeof(char));
	vsprintf(formattedText, text, vaFormat);
	va_end(vaFormat);

	return Button(renderer, rect, formattedText);
}

void Button_function(SDL_Renderer *renderer, SDL_Rect rect, char *text, ButtonFunction function){
	if(Button(renderer, rect, text)){
		function();
	}
}