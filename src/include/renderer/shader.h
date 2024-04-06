#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <renderer/shaderdata.h>

typedef GLuint shader_t;

GLuint shader_load(shaderdata_t sd);
void shader_destroy(shader_t shader);

#endif
