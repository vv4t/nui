#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#include <glad/glad.h>

class uniform_buffer_t {
private:
  GLuint m_ubo;
  int m_binding;
  const char *m_name;

public:
  uniform_buffer_t(int binding, const char *name, int size);
  ~uniform_buffer_t();
  void sub(void* data, int offset, int size);
  int get_binding() const;
  const char* get_name() const;
};

#endif
