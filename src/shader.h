#ifndef SHADER_H
#define SHADER_H

#include <sstream>
#include <glad/glad.h>
#include "uniform_buffer.h"

class shader_t {
private:
  GLuint m_program;

public:
  shader_t(std::stringstream& src_vertex, std::stringstream& src_fragment);
  ~shader_t();
  void bind();
  void attach(const uniform_buffer_t& uniform_buffer);
};

#endif
