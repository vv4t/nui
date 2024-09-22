#ifndef SHADER_H
#define SHADER_H

#include <sstream>
#include <glad/glad.h>

class shader_t {
private:
  GLuint m_program;

public:
  shader_t(const std::stringstream& src_vertex, const std::stringstream& src_fragment);
  ~shader_t();
  void bind() const;
  GLuint get_program() const;
};

std::stringstream shader_read_source(const char* src);

#endif
