#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

class texture_t {
private:
  GLuint m_texture;
  GLuint m_type;

public:
  texture_t(const char *src);
  texture_t(int width, int height, GLuint format, GLuint type);
  void bind(int channel);
};

#endif
