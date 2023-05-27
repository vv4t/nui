#ifndef MESH_H
#define MESH_H

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
  GLuint    ptr;
  int       num_vertices;
} mesh_t;

typedef struct {
  GLuint  vbo;
  GLuint  ptr;
  int     max_vertices;
} buffer_t;

void buffer_init(buffer_t *buffer, int max_vertices);

bool buffer_new_mesh(
  buffer_t        *buffer_t,
  mesh_t          *mesh,
  const vertex_t  *vertices,
  int             num_vertices
);

#endif
