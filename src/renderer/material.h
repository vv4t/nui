#ifndef MATERIAL_H
#define MATERIAL_H

#include "../gl/gl.h"
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

const char *material_shader_ext();
void material_shader_ext_setup(GLuint shader);

#endif
