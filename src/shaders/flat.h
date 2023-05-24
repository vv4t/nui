#ifndef FLAT_H
#define FLAT_H

#include "../renderer/gl.h"
#include "../renderer/scene.h"
#include "../common/nui_math.h"

typedef struct {
  GLuint  shader;
} flat_t;

bool flat_init(flat_t *color);
void flat_bind(flat_t *color);
void flat_set_material(material_t *material);

#endif
