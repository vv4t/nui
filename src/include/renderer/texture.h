#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

typedef GLuint texture_t;

texture_t texture_load_image(const char *path);
texture_t texture_load_cubemap(const char *base, const char *ext);
texture_t texture_create(int width, int height, GLuint format, GLuint type);
void texture_bind(texture_t texture, GLuint type, int channel);
void texture_destroy(texture_t texture);

#endif
