#include "cubemap.hpp"

cubemap_t::cubemap_t() {
  /*
  glGenTextures(1, &m_texture);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);

  for (int i = 0; i < 6; i++) {
    glTexImage2D(
      GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,
      bitmap->w, bitmap->h,
      0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap->pixels
    );
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  */
}
