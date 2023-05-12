#include "gl.h"

#include "../common/log.h"

static bool shader_compile(GLuint *shader, GLuint type, const char *define, const char *src);

const char *glsl_version    = "#version 300 es";
const char *glsl_precision  = "precision mediump float;";

bool gl_init()
{
  glewExperimental = true;
  
  GLenum status = glewInit();
  if (status != GLEW_OK)
    LOG_ERROR("failed to initialize GLEW: %s", glewGetErrorString(status));
  
  return status == GLEW_OK;
}

bool texture_load(GLuint *texture, const char *path)
{
  SDL_Surface *bitmap = IMG_Load(path);
  
  if (!bitmap) {
    LOG_ERROR("could not load %s", path);
    return false;
  }
  
  glGenTextures(1, texture);
  glBindTexture(GL_TEXTURE_2D, *texture);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  GLuint internal_format = GL_RGB;
  switch (bitmap->format->BytesPerPixel) {
  case 3:
    internal_format = GL_RGB;
    break;
  case 4:
    internal_format = GL_RGBA;
    break;
  }
  
  glTexImage2D(GL_TEXTURE_2D, 0, internal_format, bitmap->w, bitmap->h, 0, internal_format, GL_UNSIGNED_BYTE, bitmap->pixels);
  
  SDL_FreeSurface(bitmap);
  
  return true;
}

bool shader_load(
  GLuint      *shader,
  const char  *define,
  const char  *src_vertex,
  const char  *src_fragment)
{
  int success;
  static GLchar info[1024];
  
  *shader = glCreateProgram();
  
  GLuint vertex_shader;
  if (src_vertex) {
    if (!shader_compile(&vertex_shader, GL_VERTEX_SHADER, define, src_vertex))
      return false;
    
    glAttachShader(*shader, vertex_shader);
  }
  
  GLuint fragment_shader;
  if (src_fragment) {
    if (!shader_compile(&fragment_shader, GL_FRAGMENT_SHADER, define, src_fragment))
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
    LOG_ERROR("failed to compiler shader\n%s\n%s", info, src);
    return false;
  }
  
  return true;
}
