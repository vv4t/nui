#include "flat.h"

#include "gl.h"
#include "shader.h"

typedef struct {
  GLuint shader;
} flat_t;

static flat_t flat;

bool flat_init()
{
  if (!shader_load(&flat.shader, "flat", "")) {
    return false;
  }
  
  glUseProgram(flat.shader);
  
  GLuint ul_color = glGetUniformLocation(flat.shader, "u_color");
  glUniform1i(ul_color, 0);
  
  GLuint ubl_matrices = glGetUniformBlockIndex(flat.shader, "ubo_matrices");
  glUniformBlockBinding(flat.shader, ubl_matrices, 0);
  
  return true;
}

void flat_bind()
{
  glUseProgram(flat.shader);
}
