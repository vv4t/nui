#ifndef LIGHT_H
#define LIGHT_H

#include "../renderer/gl.h"
#include "../renderer/view.h"
#include "../renderer/scene.h"
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
  GLuint      light_shader;
  GLuint      ubo_lights;
  
  GLuint      shadow_shader;
  
  GLuint      depth_fbo;
  GLuint      depth_map;
  
  int         light_count;
  
  scene_t     *scene;
} lights_t;

bool lights_init(lights_t *lights);
void lights_bind(lights_t *lights);

void lights_set_scene(lights_t *lights, scene_t *scene);
void lights_set_material(material_t *material);

bool lights_new_light(lights_t *lights, light_t *light);
void lights_sub_light(lights_t *lights, light_t *light);

#endif
