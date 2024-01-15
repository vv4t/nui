#ifndef LIGHT_H
#define LIGHT_H

#include "../gl/gl.h"
#include "../common/nui_math.h"

bool light_init();
void light_shader_setup(GLuint shader);
void light_shader_setup_shadow(GLuint shader);
void light_bind_depth_map(GLuint shader);
void light_sub_point(int id, vec3_t pos, float intensity, vec3_t color);

#endif
