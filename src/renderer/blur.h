#ifndef BLUR_H
#define BLUR_H

#include <stdbool.h>

bool blur_init(int width, int height);
void blur_begin();
void blur_end();
void blur_draw(int x, int y, int width, int height);

#endif
