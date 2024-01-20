#include "frame.h"

#include "camera.h"
#include "../gl/mesh.h"
#include "../gl/shader.h"
#include "../gl/quad.h"
#include "../common/path.h"

#define FRAME_MAX 2

typedef struct {
  int width;
  int height;
  GLuint buffer[FRAME_MAX];
  GLuint fbo[FRAME_MAX];
  GLuint rbo[FRAME_MAX];
} frame_t;

static frame_t frame;

void frame_init(int width, int height)
{
  for (int i = 0; i < FRAME_MAX; i++) {
    glGenTextures(1, &frame.buffer[i]);
    glBindTexture(GL_TEXTURE_2D, frame.buffer[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glGenFramebuffers(1, &frame.fbo[i]);
    glBindFramebuffer(GL_FRAMEBUFFER, frame.fbo[i]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame.buffer[i], 0);
    
    glGenTextures(1, &frame.rbo[i]);
    glBindTexture(GL_TEXTURE_2D, frame.rbo[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frame.rbo[i], 0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  
  frame.width = width;
  frame.height = height;
}

void frame_begin(int frame_num)
{
  glBindFramebuffer(GL_FRAMEBUFFER, frame.fbo[frame_num]);
  camera_set_viewport(0, 0, frame.width, frame.height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void frame_end()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void frame_draw(GLuint shader, int frame_num)
{
  glUseProgram(shader);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, frame.buffer[frame_num]);
  
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

GLuint frame_get_fbo(int frame_num)
{
  return frame.fbo[frame_num];
}
