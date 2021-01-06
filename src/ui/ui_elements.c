#include "../global.h"
#include "../render/renderer.h"
#include "../render/render_text.h"
#include "../event.h"
#include "ui.h"
#include "ui_elements.h"

bool Button(SDL_Renderer *renderer, Vector2 position, char *text){
	bool isClicked = false;

	SDL_Point mouse;
	SDL_GetMouseState(&mouse.x, &mouse.y);

	SDL_Rect rect = {position.x, position.y, 200, 32};

	if(SDL_PointInRect(&mouse, &rect)){
		rect.x -= 2;
		rect.y -= 2;
		rect.w += 4;
		rect.h += 4;
		if(mouseHeld){
			rect.x += 2;
			rect.y += 2;
			rect.w -= 4;
			rect.h -= 4;
		}
		if(mouseClicked){
			isClicked = true;
		}
	}

	PushRender_Tilesheet(renderer, FindTilesheet("ui"), 0, rect, RNDR_UI);

	Vector2 textPos = {(rect.x + rect.w / 2) - (strlen(text) * 10) / 2, (rect.y + rect.h / 2) - 8};
	RenderText(renderer, FindFont("default_font"), 1, textPos.x, textPos.y, text);

	return isClicked;
}

void Button_function(SDL_Renderer *renderer, Vector2 position, char *text, ButtonFunction function){
	if(Button(renderer, position, text)){
		function();
	}
}

void Checkbox(SDL_Renderer *renderer, bool *value, char *label, Vector2 position){
	SDL_Point mouse;
	SDL_GetMouseState(&mouse.x, &mouse.y);

	SDL_Rect checkbox = {position.x - 42, position.y, 32, 32};
	SDL_Rect hitbox = {checkbox.x + 8, checkbox.y + 8, checkbox.w - 16, checkbox.h - 16};

	if(SDL_PointInRect(&mouse, &hitbox)){
		if(mouseClicked){
			*value = !*value;
		}
	}
	PushRender_Tilesheet(renderer, FindTilesheet("ui"), !*value + 1, checkbox, RNDR_UI);
	RenderText(renderer, FindFont("default_font"), 1, position.x + 14, position.y + 8, label);
}