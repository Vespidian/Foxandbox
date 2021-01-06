#ifndef UI_ELEMENTS_H_
#define UI_ELEMENTS_H_

typedef void (*ButtonFunction)();

/**
 *  Renders a button with specified text
 *  \return Boolean specifying whether or not button is pressed
 */
bool Button(SDL_Renderer *renderer, Vector2 position, char *text);

/**
 *  Renders a button with specified text. If button is pressed 'function' is called
 */
void Button_function(SDL_Renderer *renderer, Vector2 position, char *text, ButtonFunction function);

/**
 *  Toggles 'value' whenever checkbox is triggered
 */
void Checkbox(SDL_Renderer *renderer, bool *value, char *label, Vector2 position);

#endif