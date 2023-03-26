#ifndef MATERIAL_H
#define MATERIAL_H

#include "gl.h"

typedef struct {
  GLuint  color;
  GLuint  normal;
} material_t;

bool material_init(material_t *material, const char *src_color, const char *src_normal);
void material_bind(material_t *material);

#endif
