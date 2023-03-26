#ifndef SHADER_H
#define SHADER_H

#include "gl.h"
#include <stdbool.h>

bool shader_load(
  GLuint      *shader,
  const char  *src_vertex,
  const char  *src_geometry,
  const char  *src_fragment);

#endif
