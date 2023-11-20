#include "gl.h"

#define INFO_MAX 1024

#include "../common/path.h"
#include "../common/file.h"
#include "../common/log.h"

static bool shader_compile(GLuint *shader, path_t path, GLuint type, const char *define);

const char *glsl_version = "#version 300 es";
const char *glsl_precision = "precision mediump float;";

bool shader_load(GLuint *shader, const char *name)
{
  *shader = glCreateProgram();
  
  path_t path;
  
  path_create(path, "assets/shader/%s/%s.vsh", name, name);
  GLuint vertex_shader;
  if (!shader_compile(&vertex_shader, path, GL_VERTEX_SHADER, "")) {
    return false;
  }
  
  glAttachShader(*shader, vertex_shader);
  
  path_create(path, "assets/shader/%s/%s.fsh", name, name);
  GLuint fragment_shader;
  if (!shader_compile(&fragment_shader, path, GL_FRAGMENT_SHADER, "")) {
    return false;
  }
  
  glAttachShader(*shader, fragment_shader);
  
  int success;
  static char info[INFO_MAX];
  
  glLinkProgram(*shader);
  glGetProgramiv(*shader, GL_LINK_STATUS, &success);
  
  if (!success) {
    glGetProgramInfoLog(*shader, INFO_MAX, NULL, info);
    LOG_ERROR("failed to link shader (%s):\n%s", name, info);
    return false;
  }
  
  glDetachShader(*shader, vertex_shader);
  glDeleteShader(vertex_shader);

  glDetachShader(*shader, fragment_shader);
  glDeleteShader(fragment_shader);
  
  return true;
}

static bool shader_compile(GLuint *shader, path_t path, GLuint type, const char *define)
{
  char *src = file_read_all(path);
  
  if (!src) {
    return false;
  }
  
  int success;
  static GLchar info[INFO_MAX];
  
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
    glGetShaderInfoLog(*shader, INFO_MAX, NULL, info);
    LOG_ERROR("failed to compiler shader (%s):\n%s", path, info);
    return false;
  }
  
  free(src);
  
  return true;
}
