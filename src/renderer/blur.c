#include "blur.h"

#include "frame.h"
#include "shader.h"

typedef struct {
  GLuint shader;
  GLuint ul_horizontal;
  frame_t frame[2];
} blur_t;

static blur_t blur;

bool blur_init(int width, int height)
{
  if (!fx_shader_load(&blur.shader, "blur", "")) {
    return false;
  }
  
  blur.ul_horizontal = glGetUniformLocation(blur.shader, "u_horizontal");
  
  if (!frame_new(&blur.frame[0], blur.shader, width, height)) {
    return false;
  }
  
  if (!frame_new(&blur.frame[1], blur.shader, width, height)) {
    return false;
  }
  
  return true;
}

void blur_begin()
{
  frame_begin(&blur.frame[0]);
}

void blur_end()
{
  frame_end();
  
  glUseProgram(blur.shader);
  
  for (int i = 0; i < 5; i++) {
    glUniform1i(blur.ul_horizontal, 0);
    
    frame_begin(&blur.frame[1]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    frame_draw(&blur.frame[0], 0, 0, blur.frame[1].width, blur.frame[1].height);
    frame_end();
    
    glUniform1i(blur.ul_horizontal, 1);
    
    frame_begin(&blur.frame[0]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    frame_draw(&blur.frame[1], 0, 0, blur.frame[0].width, blur.frame[0].height);
    frame_end();
  }
}

void blur_draw(int x, int y, int width, int height)
{
  frame_draw(&blur.frame[0], x, y, width, height);
}
