#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#include <glad/glad.h>
#include "shader.h"

class uniform_buffer_t {
private:
  GLuint m_ubo;
  int m_binding;
  const char *m_name;

public:
  uniform_buffer_t(int binding, const char *name, int size);
  ~uniform_buffer_t();
  void sub(void* data, int offset, int size);
  void attach_shader(const shader_t& shader);
};

#endif
