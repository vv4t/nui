#include "gl.h"

#include "../common/log.h"

bool gl_init()
{
#ifdef __EMSCRIPTEN__
  return true;
#else
  glewExperimental = true;
  
  GLenum status = glewInit();
  if (status != GLEW_OK)
    LOG_ERROR("failed to initialize GLEW: %s", glewGetErrorString(status));
  
  return status == GLEW_OK;
#endif
}

bool texture_load(GLuint *texture, const char *path)
{
  SDL_Surface *bitmap = IMG_Load(path);
  
  if (!bitmap) {
    LOG_ERROR("could not load %s", path);
    return false;
  }
  
  glGenTextures(1, texture);
  glBindTexture(GL_TEXTURE_2D, *texture);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  GLuint internal_format = GL_RGBA;
  
#ifndef __EMSCRIPTEN__
  switch (bitmap->format->BytesPerPixel) {
  case 3:
    internal_format = GL_RGB;
    break;
  case 4:
    internal_format = GL_RGBA;
    break;
  }
#endif
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap->w, bitmap->h, 0, internal_format, GL_UNSIGNED_BYTE, bitmap->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  
  SDL_FreeSurface(bitmap);
  
  return true;
}
