#ifndef FRAME_H
#define FRAME_H

#include "gl.h"

typedef struct {
  int width;
  int height;
  GLuint shader;
  GLuint buffer;
  GLuint fbo;
  GLuint rbo;
} frame_t;

bool frame_new(frame_t *frame, GLuint fx_shader, int width, int height);
void frame_begin(frame_t *frame);
void frame_end();
void frame_draw(frame_t *frame);

#endif
