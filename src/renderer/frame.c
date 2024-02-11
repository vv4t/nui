#include "frame.h"

#include "camera.h"
#include "../gl/mesh.h"
#include "../gl/shader.h"
#include "../gl/quad.h"
#include "../common/path.h"

void frame_new(frame_t *frame, int width, int height)
{
  glGenTextures(1, &frame->texture);
  glBindTexture(GL_TEXTURE_2D, frame->texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  glGenFramebuffers(1, &frame->fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, frame->fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame->texture, 0);
  
  glGenTextures(1, &frame->rbo);
  glBindTexture(GL_TEXTURE_2D, frame->rbo);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frame->rbo, 0);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  frame->width = width;
  frame->height = height;
}

void frame_begin(frame_t frame)
{
  glBindFramebuffer(GL_FRAMEBUFFER, frame.fbo);
  camera_set_viewport(0, 0, frame.width, frame.height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void frame_end()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void frame_draw(GLuint shader, frame_t frame)
{
  glUseProgram(shader);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, frame.texture);
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  quad_draw();
}

void frame_shader_source(shader_setup_t *shader_setup, const char *name)
{
  path_t path_frame;
  path_create(path_frame, "assets/shader/frame/%s.frag", name);
  
  shader_setup_import(shader_setup, SHADER_FRAGMENT, "frame");
  shader_setup_source_each(shader_setup, "assets/shader/frame/frame.vert", path_frame); 
}

bool frame_shader_load(GLuint *shader, const char *name)
{
  shader_setup_t shader_setup;
  shader_setup_init(&shader_setup, name);
  frame_shader_source(&shader_setup, name);
  
  if (!shader_setup_compile(shader, &shader_setup)) {
    return false;
  }
  
  shader_setup_free(&shader_setup);
  
  return true;
}

void fbo_blit(GLuint dst, GLuint src, GLuint mask)
{
  glBindFramebuffer(GL_READ_FRAMEBUFFER, src);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst);
  glBlitFramebuffer(0, 0, 1280, 720, 0, 0, 1280, 720, mask, GL_NEAREST);
}
