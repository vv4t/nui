#ifndef LIGHT_H
#define LIGHT_H

#define MAX_LIGHTS 8

typedef struct {
  vec3_t  pos;
  float   intensity;
  vec4_t  color;
} light_t;

typedef struct {
  light_t lights[MAX_LIGHTS];
} ubo_lights_t;

typedef struct {
  vec3_t 
} point_light_t;

typedef struct {
  GLuint  light_shader;
  GLuint  shadow_shader;
  
  GLuint  ubo_lights;
  light_t lights[MAX_LIGHTS];
  
  GLuint  depth_map_fbo;
  GLuint  depth_cube_map[MAX_LIGHTS];
} lighting_t;

void lighting_init(lighting_t *lighting);
void lighting_bind(lighting_t *lighting);

void lighting_set_light(lighting_t *lighting, int light_id, vec3_t pos, float intensity, vec4_t color);
void lighting_update_light(lighting_t *lighting, int light_id);
void lighting_shadow_pass(lighting_t *lighting, int light_id);

#endif
