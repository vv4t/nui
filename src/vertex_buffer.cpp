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
  
  mesh_t mesh = mesh_t(m_offset, (int) vertices.size());
  
  m_offset += (int) vertices.size();
  bind();
  glBufferSubData(
    GL_ARRAY_BUFFER,
    mesh.get_offset() * sizeof(vertex_t),
    mesh.get_count() * sizeof(vertex_t),
    vertices.data()
  );
  
  return mesh;
}

vertex_buffer_t::~vertex_buffer_t() {
  glDeleteVertexArrays(1, &m_vao);
  glDeleteBuffers(1, &m_vbo);
}

mesh_t::mesh_t(int offset, int count) {
  m_offset = offset;
  m_count = count;
}

void mesh_t::draw() {
  glDrawArrays(GL_TRIANGLES, m_offset, m_count);
}

int mesh_t::get_offset() {
  return m_offset;
}

int mesh_t::get_count() {
  return m_count;
}
