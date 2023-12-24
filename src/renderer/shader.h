#ifndef SHADER_H
#define SHADER_H

#include "gl.h"

bool forward_shader_load(GLuint *shader, const char *name);
bool shader_load(GLuint *shader, const char *name);
bool fx_shader_load(GLuint *shader, const char *name);
bool defer_shader_load(GLuint *shader, const char *name);

#endif
