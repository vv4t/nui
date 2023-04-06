#ifndef LIGHT_H
#define LIGHT_H

#include "gl.h"
#include "nui_math.h"
#include "material.h"

#define MAX_LIGHTS 8

typedef struct {
  vec3_t    pos;
  float     intensity;
  vec4_t    color;
} light_t;

typedef struct {
  light_t   lights[MAX_LIGHTS];
} ub_lights_t;

typedef struct {
  GLuint      light_shader;
  GLuint      ubo_lights;
  GLuint      depth_fbo;
  GLuint      depth_map;
} lights_t;

bool lights_init(lights_t *lights);
void lights_bind(lights_t *lights);
void lights_bind_material(material_t *material);
void lights_set_light(lights_t *lights, int light_id, vec3_t pos, float intensity, vec4_t color);

#endif
