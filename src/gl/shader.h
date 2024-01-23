#ifndef SHADER_H
#define SHADER_H

#include "gl.h"

typedef struct {
  const char *name;
  
  char *vert_src[64];
  int num_vert_src;
  
  char *frag_src[64];
  int num_frag_src;
} shader_setup_t;

typedef enum {
  SHADER_VERTEX = 1,
  SHADER_FRAGMENT = 2,
  SHADER_BOTH = SHADER_VERTEX | SHADER_FRAGMENT
} shader_flag_t;

void shader_setup_init(shader_setup_t *shader_setup, const char *name);
void shader_setup_use(shader_setup_t *shader_setup, shader_flag_t shaders, const char *text);
bool shader_setup_import(shader_setup_t *shader_setup, shader_flag_t shaders, const char *shader_import);
bool shader_setup_source(shader_setup_t *shader_setup, const char *path, const char *name);
bool shader_setup_source_each(shader_setup_t *shader_setup, const char *vert_path, const char *frag_path);
bool shader_setup_compile(GLuint *shader, const shader_setup_t *shader_setup);
void shader_setup_free(shader_setup_t *shader_setup);

bool shader_load(GLuint *shader, const char *name, const char *vsh_src[], const char *fsh_src[]);

#endif
