#include "texture.h"

#include "log.h"
#include <SDL2/SDL_image.h>

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
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmap->w, bitmap->h, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap->pixels);
  
  SDL_FreeSurface(bitmap);
  
  return true;
}
