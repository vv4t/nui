#include "light.h"

#include "../common/file.h"
#include "gl.h"

typedef struct {
  GLuint shader;
} light_t;

static light_t light;

bool light_init()
{
  if (!shader_load(&light.shader, "light")) {
    STACK_TRACE();
    return false;
  }
  
  GLuint ul_color = glGetUniformLocation(light.shader, "u_color");
  GLuint ubl_matrices = glGetUniformBlockIndex(light.shader, "ubo_matrices");
  
  glUseProgram(light.shader);
  
  glUniform1i(ul_color, 0);
  glUniformBlockBinding(light.shader, ubl_matrices, 0);
  
  return true;
}

void light_bind()
{
  glUseProgram(light.shader);
}
