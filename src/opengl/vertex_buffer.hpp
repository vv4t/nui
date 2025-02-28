#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include <glad/glad.h>
#include <vector>
#include "vertex.hpp"

class mesh_t {
private:
  int m_offset;
  int m_count;
public:
  mesh_t();
  mesh_t(int offset, int count);
  void draw();
};

class vertex_buffer_t {
private:
  GLuint m_vao;
  GLuint m_vbo;
  int m_offset;
  int m_max_vertices;

public:
  vertex_buffer_t(int max_vertices);
  ~vertex_buffer_t();
  void bind();
  mesh_t push(std::vector<vertex_t> vertices);
};

#endif
