#include "defer.h"

#include "camera.h"
#include "material.h"
#include "api.h"
#include "../gl/gl.h"
#include "../gl/quad.h"
#include "../gl/shader.h"
#include "../common/path.h"

typedef struct {
  int width;
  int height;
  GLuint g_buffer;
  GLuint g_pos;
  GLuint g_normal;
  GLuint g_albedo;
  GLuint rbo;
  GLuint shader;
} defer_t;

static defer_t defer;

bool defer_init(int width, int height)
{
  glGenFramebuffers(1, &defer.g_buffer);
  glBindFramebuffer(GL_FRAMEBUFFER, defer.g_buffer);
    
  glGenTextures(1, &defer.g_pos);
  glBindTexture(GL_TEXTURE_2D, defer.g_pos);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, defer.g_pos, 0);
  
  glGenTextures(1, &defer.g_normal);
  glBindTexture(GL_TEXTURE_2D, defer.g_normal);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, defer.g_normal, 0);
    
  glGenTextures(1, &defer.g_albedo);
  glBindTexture(GL_TEXTURE_2D, defer.g_albedo);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, defer.g_albedo, 0);
    
  unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
  glDrawBuffers(3, attachments);
  
  glGenTextures(1, &defer.rbo);
  glBindTexture(GL_TEXTURE_2D, defer.rbo);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, defer.rbo, 0);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  shader_setup_t shader_setup;
  shader_setup_init(&shader_setup, "g_buffer");
  shader_setup_import(&shader_setup, SHADER_BOTH, "camera");
  shader_setup_import(&shader_setup, SHADER_BOTH, "material");
  
  if (!shader_setup_source(&shader_setup, "assets/shader/g_buffer", "g_buffer")) {
    return false;
  }
  
  if (!shader_setup_compile(&defer.shader, &shader_setup)) {
    return false;
  }
  
  camera_shader_setup(defer.shader);
  material_shader_setup(defer.shader);
  
  defer.width = width;
  defer.height = height;
  
  return true;
}

void defer_begin()
{
  glBindFramebuffer(GL_FRAMEBUFFER, defer.g_buffer);
  glUseProgram(defer.shader);
  camera_set_viewport(0, 0, defer.width, defer.height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  static const float transparent[] = { 0, 0, -1, 0 };
  glClearBufferfv(GL_COLOR, 0, transparent);
}

void defer_end()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void defer_draw(GLuint shader)
{
  glUseProgram(shader);
  
  glActiveTexture(GL_TEXTURE0 + G_BUFFER_POS_BINDING);
  glBindTexture(GL_TEXTURE_2D, defer.g_pos);
  
  glActiveTexture(GL_TEXTURE0 + G_BUFFER_NORMAL_BINDING);
  glBindTexture(GL_TEXTURE_2D, defer.g_normal);
  
  glActiveTexture(GL_TEXTURE0 + G_BUFFER_ALBEDO_BINDING);
  glBindTexture(GL_TEXTURE_2D, defer.g_albedo);
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  quad_draw();
}

bool defer_shader_source(shader_setup_t *shader_setup, const char *path, const char *name)
{
  path_t path_defer;
  path_create(path_defer, "%s/%s.frag", path, name);
  
  shader_setup_import(shader_setup, SHADER_FRAGMENT, "defer");
  
  return shader_setup_source_each(shader_setup, "assets/shader/defer/defer.vert", path_defer); 
}

void defer_shader_setup(GLuint shader)
{
  glUseProgram(shader);
  
  GLuint ul_pos = glGetUniformLocation(shader, "u_pos");
  GLuint ul_normal = glGetUniformLocation(shader, "u_normal");
  GLuint ul_albedo = glGetUniformLocation(shader, "u_albedo");
  
  glUniform1i(ul_pos, G_BUFFER_POS_BINDING);
  glUniform1i(ul_normal, G_BUFFER_NORMAL_BINDING);
  glUniform1i(ul_albedo, G_BUFFER_ALBEDO_BINDING);
}

GLuint defer_get_fbo()
{
  return defer.g_buffer;
}
