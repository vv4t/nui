#ifndef HDR_H
#define HDR_H

#include <stdbool.h>

bool hdr_init(int width, int height);
void hdr_begin();
void hdr_end();
void hdr_draw(int x, int y, int width, int height);

#endif
