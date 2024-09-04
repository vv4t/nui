#ifndef TARGET_H
#define TARGET_H

#include <glad/glad.h>

class target_t {
private:
  GLuint m_framebuffer;

public:
  target_t(int num_bindings, ...);
  void bind();
  void unbind();
};

#endif
