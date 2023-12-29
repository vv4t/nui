#include "mesh.h"

#include "../common/log.h"

typedef struct {
  GLuint  vbo;
  GLuint  offset;
  int     max_vertices;
} mesh_buffer_t;

static mesh_buffer_t mesh_buffer;

void mesh_buffer_init(int max_vertices)
{
  glGenBuffers(1, &mesh_buffer.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, mesh_buffer.vbo);
  glBufferData(GL_ARRAY_BUFFER, max_vertices * sizeof(vertex_t), 0, GL_STATIC_DRAW);
  
  // pos
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (float*) 0);
  
  // tangent
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (float*) 0 + 3);
  
  // bitangent 
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (float*) 0 + 6);
  
  // normal
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (float*) 0 + 9);
  
  // uv
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (float*) 0 + 12);
  
  mesh_buffer.offset = 0;
  mesh_buffer.max_vertices = max_vertices;
}

bool mesh_buffer_new(mesh_t *mesh, const vertex_t  *vertices, int num_vertices)
{
  if (mesh_buffer.offset + num_vertices > mesh_buffer.max_vertices) {
    LOG_ERROR("ran out of memory %i/%i", mesh_buffer.offset + num_vertices, mesh_buffer.max_vertices);
    return false;
  }
  
  mesh->offset = mesh_buffer.offset;
  mesh->count = num_vertices;
  mesh_buffer.offset += num_vertices;
  
  glBufferSubData(
    GL_ARRAY_BUFFER,
    mesh->offset * sizeof(vertex_t),
    num_vertices * sizeof(vertex_t),
    vertices
  );
  
  return true;
}

bool mesh_buffer_sub(mesh_t *mesh, const vertex_t *vertices, int offset, int num_vertices)
{
  if (offset + num_vertices > mesh->count) {
    LOG_ERROR("mesh sub too big %i/%i", offset + num_vertices, mesh->count);
    return false;
  }
  
  glBufferSubData(
    GL_ARRAY_BUFFER,
    (mesh->offset + offset) * sizeof(vertex_t),
    num_vertices * sizeof(vertex_t),
    vertices
  );
  
  return true;
}

void mesh_draw(mesh_t mesh)
{
  glDrawArrays(GL_TRIANGLES, mesh.offset, mesh.count);
}
