#ifndef QUAD_BUFFER_H
#define QUAD_BUFFER_H

#include "mesh.h"
#include <stdbool.h>

typedef struct {
  vertex_t *vertices;
  int vertex_num;
  int max_vertices;
} quad_buffer_t;

void quad_buffer_init(quad_buffer_t *quad_buffer, int max_quads);
void quad_buffer_free(quad_buffer_t *quad_buffer);
bool quad_buffer_add(quad_buffer_t *quad_buffer, vec2_t pos, vec2_t size, vec2_t uv_pos, vec2_t uv_size);
void quad_buffer_reset(quad_buffer_t *quad_buffer);

#endif
