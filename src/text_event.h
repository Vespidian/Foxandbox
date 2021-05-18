#ifndef TEXT_EVENT_H_
#define TEXT_EVENT_H_

extern bool text_input;
extern char *text_buffer;
extern int text_buffer_size;
void InitTextEvent();
void PollText(SDL_Event *event);
void Emptytext_buffer();

#endif