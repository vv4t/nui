#ifndef RENDERER_API_H
#define RENDERER_API_H

#include "../gl/gl.h"

void renderer_shadow_pass();
void renderer_scene_pass();

bool custom_shader_load(GLuint *shader, const char *name, const char *ext[]);
bool forward_shader_load(GLuint *shader, const char *name);
bool fx_shader_load(GLuint *shader, const char *name);
bool defer_shader_load(GLuint *shader, const char *name);

#endif
