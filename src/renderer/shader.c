#include "shader.h"

bool shader_load(GLuint *shader, const char *name)
{
  path_t path;
  path_create(&path, "mdl", name);
  
  char *src_vertex = file_read_all("assets/shader/light.vsh");
  char *src_fragment = file_read_all("assets/shader/light.fsh");
  
  int success;
  static GLchar info[1024];
  
  *shader = glCreateProgram();
  
  GLuint vertex_shader;
  if (src_vertex) {
    if (!shader_compile(&vertex_shader, GL_VERTEX_SHADER, define, src_vertex)) {
      return false;
    }
    
    glAttachShader(*shader, vertex_shader);
  }
  
  GLuint fragment_shader;
  if (src_fragment) {
    if (!shader_compile(&fragment_shader, GL_FRAGMENT_SHADER, define, src_fragment)) {
      return false;
    }
    
    glAttachShader(*shader, fragment_shader);
  }
  
  glLinkProgram(*shader);
  glGetProgramiv(*shader, GL_LINK_STATUS, &success);
  
  if (!success) {
    glGetProgramInfoLog(*shader, 1024, NULL, info);
    LOG_ERROR("failed to link shader\n%s", info);
    return false;
  }
  
  if (src_vertex) {
    glDetachShader(*shader, vertex_shader);
    glDeleteShader(vertex_shader);
  }
  
  if (src_fragment) {
    glDetachShader(*shader, fragment_shader);
    glDeleteShader(fragment_shader);
  }
  
  return true;
}

static bool shader_compile(GLuint *shader, GLuint type, const char *define, const char *src)
{
  int success;
  static GLchar info[1024];
  
  const char *full_src[] = {
    glsl_version, "\n",
    glsl_precision, "\n",
    define, "\n",
    src
  };
  
  *shader = glCreateShader(type);
  glShaderSource(*shader, 7, full_src, NULL);
  
  glCompileShader(*shader);
  glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(*shader, 1024, NULL, info);
    LOG_ERROR("failed to compiler shader\n%s", info);
    return false;
  }
  
  return true;
}
