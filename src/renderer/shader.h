#ifndef SHADER_H
#define SHADER_H

#include "gl.h"

bool shader_load(GLuint *shader, const char *name, const char *define);
bool fx_shader_load(GLuint *shader, const char *name, const char *define);
bool defer_shader_load(GLuint *shader, const char *name, const char *define);

#endif
