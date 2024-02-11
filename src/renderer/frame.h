#ifndef FRAME_H
#define FRAME_H

#include "../gl/gl.h"
#include "../gl/shader.h"

typedef struct {
  GLuint fbo;
  GLuint rbo;
  GLuint texture;
  int width;
  int height;
} frame_t;

void frame_new(frame_t *frame, int width, int height);

void frame_begin(frame_t frame);
void frame_end();
void frame_draw(GLuint shader, frame_t frame);

void frame_shader_source(shader_setup_t *shader_setup, const char *name);
bool frame_shader_load(GLuint *shader, const char *name);

void fbo_blit(GLuint dst, GLuint src, GLuint mask);

#endif
