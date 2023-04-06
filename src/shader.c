#include "shader.h"

static bool shader_compile(GLuint *shader, GLuint type, const char *src);
static bool shader_link(GLuint *shader, GLuint vertex_shader, GLuint fragment_shader);

bool shader_load(
  GLuint      *shader,
  const char  *src_vertex,
  const char  *src_geometry,
  const char  *src_fragment)
{
  int success;
  static GLchar info[1024];
  
  *shader = glCreateProgram();
  
  GLuint vertex_shader;
  if (src_vertex) {
    if (!shader_compile(&vertex_shader, GL_VERTEX_SHADER, src_vertex))
      return false;
    
    glAttachShader(*shader, vertex_shader);
  }
  
  GLuint geometry_shader;
  if (src_geometry) {
    if (!shader_compile(&geometry_shader, GL_GEOMETRY_SHADER, src_geometry))
      return false;
    
    glAttachShader(*shader, geometry_shader);
  }
  
  GLuint fragment_shader;
  if (src_fragment) {
    if (!shader_compile(&fragment_shader, GL_FRAGMENT_SHADER, src_fragment))
      return false;
    
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
  
  if (src_geometry) {
    glDetachShader(*shader, geometry_shader);
    glDeleteShader(geometry_shader);
  }
  
  if (src_fragment) {
    glDetachShader(*shader, fragment_shader);
    glDeleteShader(fragment_shader);
  }
  
  return true;
}

static bool shader_compile(GLuint *shader, GLuint type, const char *src)
{
  int success;
  static GLchar info[1024];
  
  *shader = glCreateShader(type);
  glShaderSource(*shader, 1, &src, NULL);
  
  glCompileShader(*shader);
  glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(*shader, 1024, NULL, info);
    LOG_ERROR("failed to compiler shader\n%s", info);
    return false;
  }
  
  return true;
}

static bool shader_link(GLuint *shader, GLuint vertex_shader, GLuint fragment_shader)
{
  return true;
}
