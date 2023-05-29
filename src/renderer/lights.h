#ifndef LIGHT_H
#define LIGHT_H

#include "gl.h"
#include "renderer_def.h"
#include "../common/nui_math.h"

#define MAX_LIGHTS 4

typedef struct {
  mat4x4_t  light_matrices[6];
  vec3_t    pos;
  float     intensity;
  vec4_t    color;
} ubc_light_t;

typedef struct {
  ubc_light_t lights[MAX_LIGHTS];
} ubc_lights_t;

typedef struct {
  int     id;
  vec3_t  pos;
  vec4_t  color;
  float   intensity;
} light_t;

typedef struct {
  GLuint        light_shader;
  GLuint        ubo_lights;
  GLuint        ul_view_pos;
  
  GLuint        shadow_shader;
  
  GLuint        depth_fbo;
  GLuint        depth_map;
  
  int           light_count;
} lights_t;

bool lights_init(lights_t *lights);
void lights_bind(lights_t *lights);

void lights_set_material(material_t material);
void lights_set_view_pos(lights_t *lights, vec3_t view_pos);

bool lights_new_light(lights_t *lights, light_t *light);
void lights_sub_light(lights_t *lights, const light_t *light, const scene_t *scene, view_t view);

#endif
