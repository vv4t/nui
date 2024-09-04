#include "target.h"
#include <stdarg.h>
#include <vector>

target_t::target_t(int num_bindings, ...)
{
  glGenFramebuffers(1, &m_framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
  
  std::vector<GLuint> buffers;
  
  va_list vargs;
  va_start(vargs, num_bindings);
  
  for (int i = 0; i < num_bindings; i++) {
    GLuint attachment = va_arg(vargs, GLuint);
    GLuint texture = va_arg(vargs, GLuint);
    
    if (attachment != GL_DEPTH_ATTACHMENT) {
      buffers.push_back(attachment);
    }
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, 0);
  }
  
  va_end(vargs);
  
  glDrawBuffers(buffers.size(), buffers.data());
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void target_t::bind() {
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

void target_t::unbind() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
