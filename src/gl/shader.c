#include "shader.h"

#define INFO_MAX 1024

#include "../common/log.h"
#include "../common/path.h"
#include "../common/file.h"

typedef struct {
  const char *src;
  const char *head;
  const char *tail;
} cfg_shader_t;

static bool shader_compile(GLuint *shader, const char *name, const char *src[], GLuint type);

bool shader_load_from_file(GLuint *shader, const char *name)
{
  path_t path_vert;
  path_t path_frag;
  
  path_create(path_vert, "assets/shader/%s/%s.vsh", name, name);
  path_create(path_frag, "assets/shader/%s/%s.fsh", name, name);
  
  char *vsh = file_read_all(path_vert);
  
  if (!vsh) {
    return false;
  }
  
  char *fsh = file_read_all(path_frag);
  
  if (!fsh) {
    return false;
  }
  
  const char *vsh_src[] = {
    "#version 300 es\n",
    "precision mediump float;\n",
    vsh,
    NULL
  };
  
  const char *fsh_src[] = {
    "#version 300 es\n",
    "precision mediump float;\n",
    fsh,
    NULL
  };
  
  if (!shader_load(shader, name, vsh_src, fsh_src)) {
    return false;
  }
}

bool shader_load(GLuint *shader, const char *name, const char *vsh_src[], const char *fsh_src[])
{
  *shader = glCreateProgram();
  
  GLuint vsh;
  if (!shader_compile(&vsh, name, vsh_src, GL_VERTEX_SHADER)) {
    return false;
  }
  
  GLuint fsh;
  if (!shader_compile(&fsh, name, fsh_src, GL_FRAGMENT_SHADER)) {
    return false;
  }
  
  glAttachShader(*shader, vsh);
  glAttachShader(*shader, fsh);
  
  int success;
  static char info[INFO_MAX];
  
  glLinkProgram(*shader);
  glGetProgramiv(*shader, GL_LINK_STATUS, &success);
  
  if (!success) {
    glGetProgramInfoLog(*shader, INFO_MAX, NULL, info);
    LOG_ERROR("failed to link shader (%s):\n%s", name, info);
    return false;
  }
  
  glDetachShader(*shader, vsh);
  glDeleteShader(vsh);

  glDetachShader(*shader, fsh);
  glDeleteShader(fsh);
  
  return true;
}

static bool shader_compile(GLuint *shader, const char *name, const char *src[], GLuint type)
{
  int success;
  static GLchar info[INFO_MAX];
  
  int num_src = 0;
  
  while (src[num_src]) {
    num_src++;
  }
  
  *shader = glCreateShader(type);
  glShaderSource(*shader, num_src, src, NULL);
  
  glCompileShader(*shader);
  glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
  
  if (!success) {
    const char *type_str;
    
    switch (type) {
    case GL_VERTEX_SHADER:
      type_str = "VERTEX_SHADER";
      break;
    case GL_FRAGMENT_SHADER:
      type_str = "FRAGMENT_SHADER";
      break;
    default:
      type_str = "UNKNOWN";
      break;
    }
    
    glGetShaderInfoLog(*shader, INFO_MAX, NULL, info);
    LOG_ERROR("failed to compiler shader (%s | %s):\n\n%s", name, type_str, info);
    
    return false;
  }
  
  return true;
}
