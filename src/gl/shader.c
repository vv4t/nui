#include "shader.h"

#define INFO_MAX 1024

#include "../common/log.h"
#include "../common/path.h"
#include "../common/file.h"

static bool shader_compile(GLuint *shader, const char *name, const char *src[], GLuint type);

void shader_setup_init(shader_setup_t *shader_setup, const char *name)
{
  shader_setup->name = name;
  
  shader_setup->vert_src[0] = strdup("#version 300 es\n");
  shader_setup->vert_src[1] = strdup("precision mediump float;\n");
  shader_setup->vert_src[2] = NULL;
  shader_setup->num_vert_src = 2;
  
  shader_setup->frag_src[0] = strdup("#version 300 es\n");
  shader_setup->frag_src[1] = strdup("precision mediump float;\n");
  shader_setup->frag_src[2] = NULL;
  shader_setup->num_frag_src = 2;
}

void shader_setup_use(shader_setup_t *shader_setup, shader_flag_t shaders, const char *text)
{
  if ((shaders & SHADER_VERTEX) > 0) {
    shader_setup->vert_src[shader_setup->num_vert_src++] = strdup(text);
    shader_setup->vert_src[shader_setup->num_vert_src] = NULL;
  }
  
  if ((shaders & SHADER_FRAGMENT) > 0) {
    shader_setup->frag_src[shader_setup->num_frag_src++] = strdup(text);
    shader_setup->frag_src[shader_setup->num_frag_src] = NULL;
  }
}

bool shader_setup_import(shader_setup_t *shader_setup, shader_flag_t shaders, const char *shader_import)
{
  path_t path_import;
  path_create(path_import, "assets/shader/import/%s.glsl", shader_import);
  
  char *import_text = file_read_all(path_import);
  
  if (!import_text) {
    return false;
  }
  
  shader_setup_use(shader_setup, shaders, import_text);
  
  return true;
}

bool shader_setup_source_each(shader_setup_t *shader_setup, const char *vert_path, const char *frag_path)
{
  char *vert_src = file_read_all(vert_path);
  
  if (!vert_src) {
    return false;
  }
  
  char *frag_src = file_read_all(frag_path);
  
  if (!frag_src) {
    return false;
  }
  
  shader_setup->vert_src[shader_setup->num_vert_src++] = vert_src;
  shader_setup->vert_src[shader_setup->num_vert_src] = NULL;
  
  shader_setup->frag_src[shader_setup->num_frag_src++] = frag_src;
  shader_setup->frag_src[shader_setup->num_frag_src] = NULL;
  
  return true;
}

bool shader_setup_source(shader_setup_t *shader_setup, const char *path, const char *name)
{
  path_t path_vert;
  path_t path_frag;
  
  path_create(path_vert, "%s/%s.vert", path, name);
  path_create(path_frag, "%s/%s.frag", path, name);
  
  return shader_setup_source_each(shader_setup, path_vert, path_frag);
}

bool shader_setup_compile(GLuint *shader, const shader_setup_t *shader_setup)
{
  return shader_load(
    shader,
    shader_setup->name,
    (const char **) shader_setup->vert_src,
    (const char **) shader_setup->frag_src
  );
}

void shader_setup_free(shader_setup_t *shader_setup)
{
  for (int i = 0; i < shader_setup->num_vert_src; i++) {
    free(shader_setup->vert_src[i]);
  }
  
  for (int i = 0; i < shader_setup->num_frag_src; i++) {
    free(shader_setup->frag_src[i]);
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
