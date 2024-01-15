#ifndef DEFER_H
#define DEFER_H

#include <stdbool.h>
#include "../gl/shader.h"

bool defer_init(int width, int height);
void defer_begin();
void defer_end();
void defer_bind();
void defer_shader_source(shader_setup_t *shader_setup, const char *name);
void defer_shader_setup(GLuint shader);

#endif
