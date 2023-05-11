#ifndef MESH_H
#define MESH_H

#include "scene.h"
#include "gl.h"
#include "../common/nui_math.h"

typedef struct {
  GLuint  vbo;
  GLuint  ptr;
  int     max_vertices;
} vertex_buffer_t;

void vertex_buffer_init(vertex_buffer_t *vertex_buffer, int max_vertices);

bool vertex_buffer_new_mesh(
  vertex_buffer_t *vertex_buffer,
  mesh_t          *mesh,
  const vertex_t  *vertices,
  int             num_vertices
);

#endif
