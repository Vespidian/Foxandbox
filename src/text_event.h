#ifndef TEXT_EVENT_H_
#define TEXT_EVENT_H_

extern bool textInput;
extern char *textBuffer;
extern int textBufferSize;
void InitTextEvent();
void PollText(SDL_Event *event);
void EmptyTextBuffer();

#endif