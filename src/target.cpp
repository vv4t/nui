#include "target.h"
#include <vector>

target_t::target_t(std::vector<binding_t> bindings) {
  glGenFramebuffers(1, &m_framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
  
  std::vector<GLuint> buffers;
  for (binding_t& binding : bindings) {
    if (binding.attachment != GL_DEPTH_ATTACHMENT) {
      buffers.push_back(binding.attachment);
    }
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, binding.attachment, GL_TEXTURE_2D, binding.texture.get_texture(), 0);
  }
  
  glDrawBuffers(buffers.size(), buffers.data());
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void target_t::bind() {
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

void target_t::unbind() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

target_t::~target_t() {
  glDeleteFramebuffers(1, &m_framebuffer);
}
