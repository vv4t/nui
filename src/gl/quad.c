#include "quad.h"

#include "mesh.h"

typedef struct {
  mesh_t mesh;
  GLuint shader;
} gl_quad_t;

static gl_quad_t gl_quad;

bool quad_init()
{
  vertex_t vertices[] = {
    { .pos = { -1.0f, -1.0f, 0.0f }, .uv = { 0.0f, 0.0f } },
    { .pos = { -1.0f, +1.0f, 0.0f }, .uv = { 0.0f, 1.0f } },
    { .pos = { +1.0f, -1.0f, 0.0f }, .uv = { 1.0f, 0.0f } },
    { .pos = { -1.0f, +1.0f, 0.0f }, .uv = { 0.0f, 1.0f } },
    { .pos = { +1.0f, +1.0f, 0.0f }, .uv = { 1.0f, 1.0f } },
    { .pos = { +1.0f, -1.0f, 0.0f }, .uv = { 1.0f, 0.0f } }
  };
  
  if (!mesh_buffer_new(&gl_quad.mesh, vertices, 6)) {
    return false;
  }
  
  return true;
}

void quad_draw()
{
  mesh_draw(gl_quad.mesh);
}
