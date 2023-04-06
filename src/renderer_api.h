#ifndef RENDERER_API_H
#define RENDERER_API_H

#include "gl.h"

typedef struct {
  GLuint  color;
  GLuint  normal;
} material_t;

bool material_load(material_t *material, const char *src_color, const char *src_normal);

#endif
