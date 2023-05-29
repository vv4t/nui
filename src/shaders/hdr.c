#include "hdr.h"

#include "../common/log.h"
#include "../common/file.h"

#define SCR_WIDTH 1280
#define SCR_HEIGHT 720

bool hdr_init(hdr_t *hdr, mesh_t quad_mesh)
{
  glGenFramebuffers(1, &hdr->fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, hdr->fbo);
  
  glGenTextures(1, &hdr->color_buffer);
  
  glBindTexture(GL_TEXTURE_2D, hdr->color_buffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdr->color_buffer, 0);

#ifdef __EMSCRIPTEN__
  glGenTextures(1, &hdr->rbo);
  glBindTexture(GL_TEXTURE_2D, hdr->rbo);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, hdr->rbo, 0);
#else
  glGenRenderbuffers(1, &hdr->rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, hdr->rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, hdr->rbo);
#endif
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  char *src_vertex = file_read_all("res/shader/hdr.vsh");
  char *src_fragment = file_read_all("res/shader/hdr.fsh");

  if (!shader_load(&hdr->shader, "", src_vertex, src_fragment)) {
    LOG_ERROR("failed to load shader");
    return false;
  }
  
  free(src_vertex);
  free(src_fragment);
  
  hdr->quad_mesh = quad_mesh;
  
  return true;
}

void hdr_begin(hdr_t *hdr)
{
  glBindFramebuffer(GL_FRAMEBUFFER, hdr->fbo);
}

void hdr_end(hdr_t *hdr)
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void hdr_draw(hdr_t *hdr)
{
  glDisable(GL_DEPTH_TEST);
  
  glUseProgram(hdr->shader);
  
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, hdr->color_buffer);
  
  glDrawArrays(GL_TRIANGLES, hdr->quad_mesh.offset, hdr->quad_mesh.count);
  
  glEnable(GL_DEPTH_TEST);
}
