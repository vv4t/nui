#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <renderer/shaderdata.h>

typedef GLuint shader_t;

shader_t shader_load(shaderdata_t sd);
void shader_config(shaderdata_t sd);
void shader_bind(shader_t shader);
void shader_destroy(shader_t shader);

#endif
