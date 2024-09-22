#include "vertex_buffer.h"
#include <iostream>

vertex_buffer_t::vertex_buffer_t(int max_vertices) {
  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);
  
  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, max_vertices * sizeof(vertex_t), 0, GL_STATIC_DRAW);
  
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (float*) 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (float*) 0 + 3);
  
  m_offset = 0;
  m_max_vertices = max_vertices;
}

void vertex_buffer_t::bind() {
  glBindVertexArray(m_vao);
}

mesh_t vertex_buffer_t::push(std::vector<vertex_t> vertices) {
  if (m_offset + (int) vertices.size() > m_max_vertices) {
    throw std::runtime_error("vertex buffer out of memory");
  }
  
  bind();
  
  int offset = m_offset;
  m_offset += (int) vertices.size();
  
  glBufferSubData(
    GL_ARRAY_BUFFER,
    offset * sizeof(vertex_t),
    (int) vertices.size() * sizeof(vertex_t),
    vertices.data()
  );
  
  return mesh_t(offset, (int) vertices.size());
}

vertex_buffer_t::~vertex_buffer_t() {
  glDeleteVertexArrays(1, &m_vao);
  glDeleteBuffers(1, &m_vbo);
}

mesh_t::mesh_t(int offset, int count) {
  m_offset = offset;
  m_count = count;
}

mesh_t::mesh_t() : mesh_t(0, 0) {
  
}

void mesh_t::draw() {
  glDrawArrays(GL_TRIANGLES, m_offset, m_count);
}
