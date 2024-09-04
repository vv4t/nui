#ifndef SHADER_H
#define SHADER_H

#include <sstream>
#include <glad/glad.h>

class shader_t {
private:
  GLuint m_program;

public:
  shader_t(std::stringstream& src_vertex, std::stringstream& src_fragment);
  void bind();
};

#endif
