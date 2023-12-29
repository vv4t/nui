#include "quad_buffer.h"

void quad_buffer_init(quad_buffer_t *quad_buffer, int max_quads)
{
  quad_buffer->max_vertices = max_quads * 6;
  quad_buffer->vertex_num = 0;
  quad_buffer->vertices = calloc(quad_buffer->max_vertices, sizeof(vertex_t));
}

void quad_buffer_reset(quad_buffer_t *quad_buffer)
{
  quad_buffer->vertex_num = 0;
  memset(quad_buffer->vertices, 0, quad_buffer->max_vertices * sizeof(vertex_t));
}

void quad_buffer_free(quad_buffer_t *quad_buffer)
{
  free(quad_buffer->vertices);
}

bool quad_buffer_add(quad_buffer_t *quad_buffer, vec2_t pos, vec2_t size, vec2_t uv_pos, vec2_t uv_size)
{
  float x0 = pos.x;
  float y0 = pos.y;
  
  float x1 = pos.x + size.x;
  float y1 = pos.y + size.y;
  
  float u0 = uv_pos.x;
  float v0 = uv_pos.y;
  
  float u1 = uv_pos.x + uv_size.x;
  float v1 = uv_pos.y + uv_size.y;
  
  if (quad_buffer->vertex_num + 6 >= quad_buffer->max_vertices) {
    return false;
  }
  
  int vertex_num = quad_buffer->vertex_num;
  
  quad_buffer->vertices[vertex_num + 0] = (vertex_t) { .pos = { x0, y0, 0.0f }, .uv = { u0, v0 } };
  quad_buffer->vertices[vertex_num + 1] = (vertex_t) { .pos = { x0, y1, 0.0f }, .uv = { u0, v1 } };
  quad_buffer->vertices[vertex_num + 2] = (vertex_t) { .pos = { x1, y0, 0.0f }, .uv = { u1, v0 } };
  quad_buffer->vertices[vertex_num + 3] = (vertex_t) { .pos = { x0, y1, 0.0f }, .uv = { u0, v1 } };
  quad_buffer->vertices[vertex_num + 4] = (vertex_t) { .pos = { x1, y1, 0.0f }, .uv = { u1, v1 } };
  quad_buffer->vertices[vertex_num + 5] = (vertex_t) { .pos = { x1, y0, 0.0f }, .uv = { u1, v0 } };
  
  quad_buffer->vertex_num += 6;
  
  return true;
}
