#ifndef CUBEMAP_H
#define CUBEMAP_H

#include "../gl/gl.h"
#include "../common/nui_math.h"
#include <stdbool.h>

bool cubemap_init();
void cubemap_pass(vec3_t view_pos);
GLuint cubemap_get_texture();

#endif
