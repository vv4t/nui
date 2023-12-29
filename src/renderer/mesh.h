#ifndef MESH_H
#define MESH_H

#include "gl.h"
#include "../common/nui_math.h"

typedef struct {
  vec3_t pos;
  vec3_t tangent;
  vec3_t bitangent;
  vec3_t normal;
  vec2_t uv;
} vertex_t;

typedef struct {
  GLuint offset;
  GLuint count;
} mesh_t;

void mesh_buffer_init(int max_vertices);
bool mesh_buffer_new(mesh_t *mesh, const vertex_t *vertices, int num_vertices);
bool mesh_buffer_sub(mesh_t *mesh, const vertex_t *vertices, int offset, int num_vertices);

void mesh_draw(mesh_t mesh);

#endif
