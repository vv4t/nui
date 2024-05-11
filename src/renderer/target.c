#include <renderer/target.h>

#include <stdio.h>
#include <stdarg.h>

#define MAX_BUFFERS 32

target_t target_create(int num_bindings, ...)
{
  target_t target;
  glGenFramebuffers(1, &target);
  glBindFramebuffer(GL_FRAMEBUFFER, target);
  
  GLuint buffers[MAX_BUFFERS];
  
  int num_draw = 0;
  
  va_list vargs;
  va_start(vargs, num_bindings);
  
  for (int i = 0; i < num_bindings; i++) {
    GLuint attachment = va_arg(vargs, GLuint);
    GLuint texture = va_arg(vargs, GLuint);
    
    if (attachment != GL_DEPTH_ATTACHMENT && i < MAX_BUFFERS) {
      buffers[i] = attachment;
      num_draw++;
    }
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, 0);
  }
  
  va_end(vargs);
  
  glDrawBuffers(num_draw, buffers);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  return target;
}

void target_bind(target_t target)
{
  glBindFramebuffer(GL_FRAMEBUFFER, target);
}

void target_unbind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void target_destroy(target_t target)
{
  glDeleteFramebuffers(1, &target);
}
