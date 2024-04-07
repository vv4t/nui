#include <renderer/vbuffer.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>

struct {
  GLuint vbo;
  int offset;
  int max_vertices;
} vbuffer;

void vbuffer_init(int max_vertices)
{
  glGenBuffers(1, &vbuffer.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbuffer.vbo);
  glBufferData(GL_ARRAY_BUFFER, max_vertices * sizeof(vertex_t), 0, GL_STATIC_DRAW);
  
  // vertex.p
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (float*) 0);
  
  // vertex.n
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (float*) 0 + 3);
  
  // vertex.t
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (float*) 0 + 6);
  
  // vertex.bt 
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (float*) 0 + 9);
  
  // vertex.uv
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (float*) 0 + 12);
  
  vbuffer.offset = 0;
  vbuffer.max_vertices = max_vertices;
}

void vbuffer_bind()
{
  glBindBuffer(GL_ARRAY_BUFFER, vbuffer.vbo);
}

mesh_t vbuffer_add(meshdata_t md)
{
  int num_vertices = meshdata_get_size(md);
  
  if (vbuffer.offset + num_vertices > vbuffer.max_vertices) {
    fprintf(stderr, "error:%s:%s:%i: vbuffer out of memory\n", __FILE__, __FUNCTION__, __LINE__);
    exit(1);
  }
  
  mesh_t mesh = {
    .offset = vbuffer.offset,
    .count = num_vertices
  };
  
  vbuffer.offset += num_vertices;
  
  glBufferSubData(
    GL_ARRAY_BUFFER,
    mesh.offset * sizeof(vertex_t),
    num_vertices * sizeof(vertex_t),
    meshdata_get_vertices(md)
  );
  
  return mesh;
}

void vbuffer_draw(mesh_t mesh)
{
  glDrawArrays(GL_TRIANGLES, mesh.offset, mesh.count);
}

void vbuffer_deinit()
{
  glDeleteBuffers(1, &vbuffer.vbo);
}
