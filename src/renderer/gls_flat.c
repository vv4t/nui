#include "gls_flat.h"

#include "../common/file.h"

bool gls_flat_init(gls_flat_t *gls_flat)
{
  char *src_vertex = file_read_all("assets/shader/flat.vsh");
  char *src_fragment = file_read_all("assets/shader/flat.fsh");
  
  if (!shader_load(&gls_flat->program, "", src_vertex, src_fragment)) {
    return false;
  }
  
  free(src_vertex);
  free(src_fragment);
  
  GLuint ul_color = glGetUniformLocation(gls_flat->program, "u_color");
  GLuint ubl_matrices = glGetUniformBlockIndex(gls_flat->program, "ubo_matrices");
  
  glUseProgram(gls_flat->program);
  
  glUniform1i(ul_color, 0);
  glUniformBlockBinding(gls_flat->program, ubl_matrices, 0);
  
  return true;
}

void gls_flat_bind(gls_flat_t *gls_flat)
{
  glUseProgram(gls_flat->program);
}
