#ifndef RENDERER_API_H
#define RENDERER_API_H

#include "gl.h"
#include "nui_math.h"

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
} ub_matrices_t;

typedef struct {
  GLuint  ptr;
  int     num_vertices;
} mesh_t;

typedef struct draw_call {
  GLuint  ubo_matrices;
  void    *data;
  void    (*draw)(struct draw_call *draw_call, mat4x4_t view_projection_matrix, vec3_t view_pos);
} draw_call_t;

bool material_load(material_t *material, const char *src_color, const char *src_normal);
void do_draw_call(draw_call_t *draw_call, mat4x4_t view_projection_matrix, vec3_t view_pos); 
void draw_mesh(mesh_t mesh);

#endif
