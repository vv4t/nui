#ifndef FULL_BRIGHT_H
#define FULL_BRIGHT_H

#include "../renderer/gl.h"
#include "../renderer/scene.h"
#include "../common/nui_math.h"

typedef struct {
  GLuint  shader;
} full_bright_t;

bool full_bright_init(full_bright_t *color);
void full_bright_bind(full_bright_t *color);
void full_bright_set_material(material_t *material);

#endif
