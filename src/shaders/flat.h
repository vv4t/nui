#ifndef FLAT_H
#define FLAT_H

#include "../ref/gl.h"
#include "../ref/ref_def.h"
#include "../common/nui_math.h"

typedef struct {
  shader_t shader;
} flat_t;

bool flat_init(flat_t *color);
void flat_bind(flat_t *color);
void flat_set_material(material_t *material);

#endif
