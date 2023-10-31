#include "flat.h"

#include "../common/file.h"

typedef struct {
  GLuint program;
} flat_t;

static flat_t flat;

bool flat_init()
{
  char *src_vertex = file_read_all("assets/shader/flat.vsh");
  char *src_fragment = file_read_all("assets/shader/flat.fsh");
  
  if (!shader_load(&flat.program, "", src_vertex, src_fragment)) {
    return false;
  }
  
  free(src_vertex);
  free(src_fragment);
  
  GLuint ul_color = glGetUniformLocation(flat.program, "u_color");
  GLuint ubl_matrices = glGetUniformBlockIndex(flat.program, "ubo_matrices");
  
  glUseProgram(flat.program);
  
  glUniform1i(ul_color, 0);
  glUniformBlockBinding(flat.program, ubl_matrices, 0);
  
  return true;
}

void flat_bind()
{
  glUseProgram(flat.program);
}
