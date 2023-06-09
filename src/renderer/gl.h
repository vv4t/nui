#ifndef GL_H
#define GL_H

#ifdef __EMSCRIPTEN__
  #include <SDL_opengles2.h>
  #include <GLES3/gl3.h>
  #include <GLES3/gl2ext.h>
#else
  #include <GL/glew.h>
#endif

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
