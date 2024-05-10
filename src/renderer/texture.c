#include <renderer/texture.h>

#include <SDL2/SDL_image.h>

static GLuint bitmap_format(SDL_Surface *bitmap);

texture_t texture_load_image(const char *path)
{
  SDL_Surface *bitmap = IMG_Load(path);
  
  if (!bitmap) {   
    fprintf(stdout, "SDL_GetError: %s\n", SDL_GetError());
    exit(1);
  }
  
  texture_t texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGBA,
    bitmap->w, bitmap->h,
    0, bitmap_format(bitmap), GL_UNSIGNED_BYTE,
    bitmap->pixels
  );
  
  glGenerateMipmap(GL_TEXTURE_2D);
  
  SDL_FreeSurface(bitmap);
  
  return texture;
}

texture_t texture_load_cubemap(const char *base, const char *ext)
{
  const char *faces[] = {
    "right",
    "left",
    "up",
    "down",
    "front",
    "back"
  };
  
  texture_t texture = GL_INVALID_VALUE;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
  
  for (int i = 0; i < 6; i++) {
    char path[256];
    snprintf(path, sizeof(path), "%s/%s.%s", base, faces[i], ext);
    
    SDL_Surface *bitmap = IMG_Load(path);
    
    if (!bitmap) {   
      fprintf(stdout, "SDL_GetError: %s\n", SDL_GetError());
      exit(1);
    }
    
    glTexImage2D(
      GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,
      bitmap->w, bitmap->h,
      0, bitmap_format(bitmap), GL_UNSIGNED_BYTE,
      bitmap->pixels
    );
    
    SDL_FreeSurface(bitmap);
  }
  
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  
  return texture;
}

texture_t texture_create(int width, int height, GLuint format, GLuint type)
{
  texture_t texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, NULL);
  
  return texture;
}

void texture_bind(texture_t texture, GLuint type, int channel)
{
  glActiveTexture(GL_TEXTURE0 + channel);
  glBindTexture(type, texture);
}

void texture_destroy(texture_t texture)
{
  glDeleteTextures(1, &texture);
}

GLuint bitmap_format(SDL_Surface *bitmap)
{
  switch (bitmap->format->BytesPerPixel) {
  case 3:
    return GL_RGB;
  case 4:
    return GL_RGBA;
  default:
    fprintf(stderr, "unknown format: %i\n", bitmap->format->BytesPerPixel);
    exit(1);
  }
}
