#include "../../global.h"
#include "../../renderer/renderer.h"
#include "../../renderer/render_text.h"
#include "../../event.h"

#include "checkbox.h"

void Checkbox(SDL_Renderer *renderer, bool *value, char *label, Vector2 position){
	SDL_Point mouse;
	SDL_GetMouseState(&mouse.x, &mouse.y);

	SDL_Rect checkbox = {position.x - 42, position.y, 32, 32};
	SDL_Rect hitbox = {checkbox.x + 8, checkbox.y + 8, checkbox.w - 16, checkbox.h - 16};

	if(SDL_PointInRect(&mouse, &hitbox)){
		if(mouse_clicked){
			*value = !*value;
		}
	}
	PushRender_Tilesheet(renderer, FindTilesheet("ui"), !*value + 1, checkbox, RNDR_UI);
	RenderText(renderer, FindFont("default_font"), 1, position.x + 14, position.y + 8, label);
}