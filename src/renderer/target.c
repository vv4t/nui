#include <renderer/target.h>

#include <stdarg.h>

target_t target_create(GLuint texture)
{
  target_t target;
  glGenFramebuffers(1, &target);
  glBindFramebuffer(GL_FRAMEBUFFER, target);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
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
