#ifndef GL_H
#define GL_H

#include <GL/glew.h>

#include <stdbool.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>

bool gl_init();
bool texture_load(GLuint *texture, const char *path);
bool shader_load(
  GLuint *shader,
  const char *define,
  const char *src_vertex,
  const char *src_fragment);

#endif
