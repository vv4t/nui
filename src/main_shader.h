#ifndef MAIN_SHADER_H
#define MAIN_SHADER_H

#include "gl.h"

typedef struct {
  GLuint  program;
} main_shader_t;

bool main_shader_init(main_shader_t *main_shader);

#endif
