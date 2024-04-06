#include <renderer/shader.h>

#include <stdio.h>
#include <stdlib.h>

GLuint shader_compile(GLuint type, const char **src, int num_src);

GLuint shader_load(shaderdata_t sd)
{
  int num_vert;
  const char **vert = shaderdata_get_vert(sd, &num_vert);
  GLuint vert_shader = shader_compile(GL_VERTEX_SHADER, vert, num_vert);
  
  int num_frag;
  const char **frag = shaderdata_get_frag(sd, &num_frag);
  GLuint frag_shader = shader_compile(GL_FRAGMENT_SHADER, frag, num_frag);
  
  GLuint shader = glCreateProgram();
  
  glAttachShader(shader, vert_shader);
  glAttachShader(shader, frag_shader);
  
  int success;
  static char info[1024];
  
  glLinkProgram(shader);
  glGetProgramiv(shader, GL_LINK_STATUS, &success);
  
  if (!success) {
    glGetShaderInfoLog(shader, sizeof(info), NULL, info);
    fprintf(stderr, "%s", info);
    exit(1);
  }
  
  glDetachShader(shader, vert_shader);
  glDetachShader(shader, frag_shader);
  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);
  
  return shader;
}

GLuint shader_compile(GLuint type, const char **src, int num_src)
{
  int success;
  static GLchar info[1024];
  
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, num_src, src, NULL);
  
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  
  if (!success) {
    glGetShaderInfoLog(shader, sizeof(info), NULL, info);
    fprintf(stderr, "%s", info);
    exit(1);
  }
  
  return shader;
}
