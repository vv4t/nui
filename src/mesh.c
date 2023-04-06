#include "mesh.h"

void vertex_buffer_init(vertex_buffer_t *vertex_buffer, int max_vertices)
{
  glGenBuffers(1, &vertex_buffer->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer-> vbo);
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
  
  vertex_buffer->ptr = 0;
  vertex_buffer->max_vertices = max_vertices;
}

bool vertex_buffer_new_mesh(
  vertex_buffer_t *vertex_buffer,
  mesh_t          *mesh,
  const vertex_t  *vertices,
  int             num_vertices)
{
  if (vertex_buffer->ptr + num_vertices > vertex_buffer->max_vertices) {
    LOG_ERROR("ran out of memory");
    return false;
  }
  
  mesh->ptr = vertex_buffer->ptr;
  mesh->num_vertices = num_vertices;
  vertex_buffer->ptr += num_vertices;
  
  glBufferSubData(
    GL_ARRAY_BUFFER,
    mesh->ptr * sizeof(vertex_t),
    num_vertices * sizeof(vertex_t),
    vertices
  );
  
  return true;
}

void mesh_draw(mesh_t mesh)
{
  glDrawArrays(GL_TRIANGLES, mesh.ptr, mesh.num_vertices);
}
