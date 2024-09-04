#ifndef TARGET_H
#define TARGET_H

#include <glad/glad.h>
#include <vector>
#include <utility>
#include "texture.h"

class binding_t {
public:
  GLuint attachment;
  texture_t& texture;
  
  inline binding_t(GLuint attachment_, texture_t& texture_)
    : texture(texture_) {
    attachment = attachment_;
  }
};

class target_t {
private:
  GLuint m_framebuffer;

public:
  target_t(std::vector<binding_t> bindings);
  ~target_t();
  void bind();
  void unbind();
};

#endif
