#ifndef MESH_H
#define MESH_H

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
  GLuint  vbo;
  GLuint  ptr;
  int     max_vertices;
} vertex_buffer_t;

typedef struct {
  GLuint  ptr;
  int     num_vertices;
} mesh_t;

void vertex_buffer_init(vertex_buffer_t *vertex_buffer, int max_vertices);

bool vertex_buffer_new_mesh(
  vertex_buffer_t *vertex_buffer,
  mesh_t          *mesh,
  const vertex_t  *vertices,
  int             num_vertices
);

#endif
