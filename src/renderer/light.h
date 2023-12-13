#ifndef LIGHT_H
#define LIGHT_H

#include "gl.h"
#include "../common/nui_math.h"

bool light_init();
void light_bind();
void light_sub_point(int id, vec3_t pos, float intensity, vec4_t color);

#endif
