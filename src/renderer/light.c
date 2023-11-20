#include "light.h"

#include "shader.h"

typedef struct {
  GLuint shader;
} light_t;

static light_t light;

bool light_init()
{
  if (!shader_load(&light.shader, "light")) {
    return false;
  }
  
  glUseProgram(light.shader);
  
  GLuint ul_color = glGetUniformLocation(light.shader, "u_color");
  glUniform1i(ul_color, 0);
  
  GLuint ubl_matrices = glGetUniformBlockIndex(light.shader, "ubo_matrices");
  glUniformBlockBinding(light.shader, ubl_matrices, 0);
  
  return true;
}

void light_bind()
{
  glUseProgram(light.shader);
}
