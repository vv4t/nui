#ifndef SHADER_H
#define SHADER_H

#include "gl.h"

bool shader_load(GLuint *shader, const char *name, const char *vsh_src[], const char *fsh_src[]);
bool shader_load_from_file(GLuint *shader, const char *name);

#endif
