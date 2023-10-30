#ifndef MATERIAL_H
#define MATERIAL_H

#include "gl.h"

typedef struct {
  texture_t diffuse;
} material_t;

void material_bind(material_t material);

#endif
