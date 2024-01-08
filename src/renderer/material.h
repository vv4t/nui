#ifndef MATERIAL_H
#define MATERIAL_H

#include "gl.h"
#include "../common/nui_math.h"

typedef struct {
  texture_t diffuse;
  texture_t normal;
  vec3_t color;
  float specular;
} material_t;

void material_init();
void material_new(material_t *material);
void material_bind(material_t material);

#endif
