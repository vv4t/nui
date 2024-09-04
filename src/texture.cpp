#include "texture.h"
#include <iostream>
#include <SDL2/SDL_image.h>

static GLuint surface_format(SDL_Surface *surface);

texture_t::texture_t(const char *src) {
  SDL_Surface *surface = IMG_Load(src);
  if (!surface) {   
    std::cerr << "error: " << src << ": " << SDL_GetError() << std::endl;
    throw std::runtime_error("failed to load image");
  }
  
  glGenTextures(1, &m_texture);
  glBindTexture(GL_TEXTURE_2D, m_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGBA,
    surface->w, surface->h,
    0, surface_format(surface), GL_UNSIGNED_BYTE,
    surface->pixels
  );
  
  glGenerateMipmap(GL_TEXTURE_2D);
  SDL_FreeSurface(surface);
  
  m_type = GL_TEXTURE_2D;
}

texture_t::texture_t(int width, int height, GLuint format, GLuint internalformat, GLuint type)
{
  glGenTextures(1, &m_texture);
  glBindTexture(GL_TEXTURE_2D, m_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, NULL);
  m_type = GL_TEXTURE_2D;
}

void texture_t::bind(int channel) {
  glActiveTexture(GL_TEXTURE0 + channel);
  glBindTexture(m_type, m_texture);
}

GLuint texture_t::get_texture() {
  return m_texture;
}

texture_t::~texture_t() {
  glDeleteTextures(1, &m_texture);
}

GLuint surface_format(SDL_Surface *surface) {
  switch (surface->format->BytesPerPixel) {
  case 3:
    return GL_RGB;
  case 4:
    return GL_RGBA;
  default:
    throw std::runtime_error("unknown surface format");
  }
}
