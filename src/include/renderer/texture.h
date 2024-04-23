#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

typedef GLuint texture_t;

texture_t texture_image(const char *path);
texture_t texture_create_rgba_byte(int width, int height);
void texture_bind(texture_t texture, GLuint type, int channel);
void texture_destroy(texture_t texture);

#endif
