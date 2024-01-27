#include "gl.h"

#include "../common/log.h"

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  if (type == GL_DEBUG_TYPE_ERROR) {
    LOG_ERROR(
      "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
      (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
      type,
      severity,
      message
    );
  }
}

bool gl_init()
{
#ifdef __EMSCRIPTEN__
  return true;
#else
  glewExperimental = true;
  
  GLenum status = glewInit();
  if (status != GLEW_OK) {
    LOG_ERROR("failed to initialize GLEW: %s", glewGetErrorString(status));
  }
  
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);
  
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
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
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
