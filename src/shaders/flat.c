#include "flat.h"

#include "../common/file.h"

bool flat_init(flat_t *flat)
{
  char *src_vertex = file_read_all("res/shader/mvp_vert_uv.vs");
  char *src_fragment = file_read_all("res/shader/flat.fs");
  
  if (!shader_load(&flat->shader, "", src_vertex, src_fragment))
    return false;
  
  free(src_vertex);
  free(src_fragment);
  
  GLuint ubl_matrices = glGetUniformBlockIndex(flat->shader, "ubo_matrices");
  GLuint ul_color = glGetUniformLocation(flat->shader, "u_color");
  
  glUseProgram(flat->shader);
  
  glUniform1i(ul_color, 0);
  glUniformBlockBinding(flat->shader, ubl_matrices, 0);
  
  return true;
}

void flat_bind(flat_t *flat)
{
  glUseProgram(flat->shader);
}

void flat_set_material(material_t *material)
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, material->diffuse);
}
