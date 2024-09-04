#include "uniform_buffer.h"
#include <iostream>

uniform_buffer_t::uniform_buffer_t(int binding, const char *name, int size) {
  m_binding = binding;
  m_name = name;
  
  glGenBuffers(1, &m_ubo);
  glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
  glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_ubo);
}

void uniform_buffer_t::sub(void* data, int offset, int size) {
  glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
  glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}
  
int uniform_buffer_t::get_binding() const {
  return m_binding;
}

const char* uniform_buffer_t::get_name() const {
  return m_name;
}

uniform_buffer_t::~uniform_buffer_t() {
  glDeleteBuffers(1, &m_ubo);
}
