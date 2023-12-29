#ifndef NGUI_H
#define NGUI_H

#include <stdbool.h>

bool ngui_init(int width, int height);
void ngui_text_input(const char *text);
void ngui_key_event(int key, int action);
void ngui_unfocus();
void ngui_render();

#endif
