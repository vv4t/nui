#ifndef BUFFER_H
#define BUFFER_H

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
  GLuint  offset;
  GLuint  count;
} mesh_t;

typedef struct {
  GLuint  vbo;
  GLuint  offset;
  int     max_vertices;
} buffer_t;

void buffer_init(buffer_t *buffer, int max_vertices);

bool buffer_new_mesh(
  buffer_t *buffer,
  mesh_t          *mesh,
  const vertex_t  *vertices,
  int             num_vertices
);

void mesh_draw(mesh_t mesh);

#endif
