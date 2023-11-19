#include "gl.h"

#include "../common/path.h"
#include "../common/file.h"
#include "../common/log.h"

static bool shader_compile(GLuint *shader, GLuint type, const char *define, const char *src);

const char *glsl_version    = "#version 300 es";
const char *glsl_precision  = "precision mediump float;";

bool shader_load(GLuint *shader, const char *name)
{
  path_t path;
  
  path_create(path, "assets/shader/%s/%s.vsh", name, name);
  
  char *src_vertex = file_read_all(path);
  if (!src_vertex) {
    return false;
  }
  
  path_create(path, "assets/shader/%s/%s.fsh", name, name);
  
  char *src_fragment = file_read_all(path);
  if (!src_fragment) {
    return false;
  }
  
  int success;
  static GLchar info[1024];
  
  *shader = glCreateProgram();
  
  GLuint vertex_shader;
  if (!shader_compile(&vertex_shader, GL_VERTEX_SHADER, "", src_vertex)) {
    return false;
  }
  
  glAttachShader(*shader, vertex_shader);
  
  GLuint fragment_shader;
  if (!shader_compile(&fragment_shader, GL_FRAGMENT_SHADER, "", src_fragment)) {
    return false;
  }
  
  glAttachShader(*shader, fragment_shader);
  
  glLinkProgram(*shader);
  glGetProgramiv(*shader, GL_LINK_STATUS, &success);
  
  if (!success) {
    glGetProgramInfoLog(*shader, 1024, NULL, info);
    LOG_ERROR("failed to link shader\n%s", info);
    return false;
  }
  
  glDetachShader(*shader, vertex_shader);
  glDeleteShader(vertex_shader);
  free(src_vertex);

  glDetachShader(*shader, fragment_shader);
  glDeleteShader(fragment_shader);
  free(src_fragment);
  
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
