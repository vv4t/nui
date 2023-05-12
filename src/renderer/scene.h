#ifndef SCENE_H
#define SCENE_H

#include "gl.h"
#include "view.h"
#include "../common/nui_math.h"

typedef struct {
  vec3_t  pos;
  vec3_t  tangent;
  vec3_t  bitangent;
  vec3_t  normal;
  vec2_t  uv;
} vertex_t;

typedef struct {
  GLuint    ptr;
  int       num_vertices;
} mesh_t;

typedef struct {
  GLuint  diffuse;
  GLuint  normal;
} material_t;

typedef struct {
  void    *data;
  view_t  *view;
  void    (*draw)(void *data, view_t *view);
} scene_t;

bool material_load(material_t *material, const char *src_diffuse, const char *src_normal);

void draw_scene(scene_t *scene); 
void draw_mesh(mesh_t mesh);

#endif
