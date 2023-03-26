#include "main_shader.h"

#include "shader.h"
#include "file.h"

bool main_shader_init(main_shader_t *main_shader)
{
  char *src_vertex = file_read_all("res/shader/main_shader.vert");
  char *src_fragment = file_read_all("res/shader/main_shader.frag");
  
  if (!shader_load(&main_shader->program, src_vertex, NULL, src_fragment))
    return false;
  
  free(src_vertex);
  free(src_fragment);
  
  GLuint ubl_matrices = glGetUniformBlockIndex(main_shader->program, "ubo_matrices");
  GLuint ubl_lights = glGetUniformBlockIndex(main_shader->program, "ubo_lights");
  
  GLuint ul_color = glGetUniformLocation(main_shader->program, "u_color");
  GLuint ul_normal = glGetUniformLocation(main_shader->program, "u_normal");
  GLuint ul_depth_map = glGetUniformLocation(main_shader->program, "u_depth_cube_map");
  
  glUseProgram(main_shader->program);
  glUniform1i(ul_color, 0);
  glUniform1i(ul_normal, 1);
  glUniform1i(ul_depth_map, 2);
  
  glUniformBlockBinding(main_shader->program, ubl_matrices, 0);
  glUniformBlockBinding(main_shader->program, ubl_lights, 1);
  
  return true;
}
