#ifndef DITHER_H
#define DITHER_H

#include <stdbool.h>

bool dither_init(int width, int height);
void dither_begin();
void dither_end();
void dither_draw();

#endif
