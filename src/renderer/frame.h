#ifndef FRAME_H
#define FRAME_H

#include "../gl/gl.h"

void frame_init(int width, int height);
void frame_begin(int frame_num);
void frame_end();
void frame_draw(GLuint shader, int frame_num);

#endif
