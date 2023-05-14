#include "full_bright.h"

#include "../common/file.h"

bool full_bright_init(full_bright_t *full_bright)
{
  char *src_vertex = file_read_all("res/shader/full_bright.vert");
  char *src_fragment = file_read_all("res/shader/full_bright.frag");
  
  if (!shader_load(&full_bright->shader, "", src_vertex, src_fragment))
    return false;
  
  free(src_vertex);
  free(src_fragment);
  
  GLuint ubl_matrices = glGetUniformBlockIndex(full_bright->shader, "ubo_matrices");
  GLuint ul_color = glGetUniformLocation(full_bright->shader, "u_color");
  
  glUseProgram(full_bright->shader);
  
  glUniform1i(ul_color, 0);
  glUniformBlockBinding(full_bright->shader, ubl_matrices, 0);
  
  return true;
}

void full_bright_bind(full_bright_t *full_bright)
{
  glUseProgram(full_bright->shader);
}

void full_bright_set_material(material_t *material)
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, material->diffuse);
}
