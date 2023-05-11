#ifndef RENDERER_API_H
#define RENDERER_API_H

#include "gl.h"
#include "../common/nui_math.h"

typedef struct {
  vec3_t  pos;
  vec3_t  tangent;
  vec3_t  bitangent;
  vec3_t  normal;
  vec2_t  uv;
} vertex_t;

typedef struct {
  GLuint  color;
  GLuint  normal;
} material_t;

typedef struct {
  mat4x4_t  mvp;
  mat4x4_t  model;
  vec3_t    view_pos;
  float     pad[1];
} ubc_view_t;

typedef struct {
  GLuint    ptr;
  int       num_vertices;
} mesh_t;

typedef struct {
  mat4x4_t  projection_matrix;
  mat4x4_t  view_projection_matrix;
  vec3_t    view_pos;
  GLuint    ubo_view;
} view_t;

typedef struct {
  void    *data;
  view_t  *view;
  void    (*draw)(void *data, view_t *view);
} scene_t;

bool material_load(material_t *material, const char *src_color, const char *src_normal);

void draw_scene(scene_t *scene); 
void draw_mesh(mesh_t mesh);

void view_init(view_t *view);

void view_perspective(view_t *view, float aspect_ratio, float fov, float near, float far);
void view_move(view_t *view, vec3_t view_offset, quat_t view_angle);
void view_set(view_t *view, mat4x4_t view_projection_matrix, vec3_t view_pos);

void view_sub_data(view_t *view, mat4x4_t model_matrix);

#endif
