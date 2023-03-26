#ifndef TEXTURE_H
#define TEXTURE_H

#include "gl.h"
#include <stdbool.h>

bool texture_load(GLuint *texture, const char *path);

#endif
