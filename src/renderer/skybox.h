#ifndef SKYBOX_H
#define SKYBOX_H

#include "../gl/gl.h"
#include <stdbool.h>

bool skybox_init(const char *name);
void skybox_render();
void skybox_render_texture(GLuint texture);
GLuint skybox_get_texture();

#endif
