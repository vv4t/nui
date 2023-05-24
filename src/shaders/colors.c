#include "colors.h"

#include "../common/file.h"

bool colors_init(colors_t *colors)
{
  char *src_vertex = file_read_all("res/shader/mvp_vert.vs");
  char *src_fragment = file_read_all("res/shader/colors.fs");
  
  if (!shader_load(&colors->shader, "", src_vertex, src_fragment))
    return false;
  
  free(src_vertex);
  free(src_fragment);
  
  colors->ul_color = glGetUniformLocation(colors->shader, "u_color");
  
  GLuint ubl_matrices = glGetUniformBlockIndex(colors->shader, "ubo_matrices");
  glUniformBlockBinding(colors->shader, ubl_matrices, 0);
  
  return true;
}

void colors_bind(colors_t *colors)
{
  glUseProgram(colors->shader);
}

void colors_set_color(colors_t *colors, vec4_t color)
{
  glUniform4f(colors->ul_color, color.x, color.y, color.z, color.w);
}
