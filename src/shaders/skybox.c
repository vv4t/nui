#include "skybox.h"

#include "../common/log.h"
#include "../common/file.h"
#include <SDL2/SDL_image.h>

static const vertex_t cube_vertices[] = {
  // Right
  { .pos = { +1.0f, -1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 1.0f } },
  { .pos = { +1.0f, -1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { +1.0f, +1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
  { .pos = { +1.0f, -1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { +1.0f, +1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 0.0f } },
  { .pos = { +1.0f, +1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
  
  // Left
  { .pos = { -1.0f, -1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { -1.0f, -1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 1.0f } },
  { .pos = { -1.0f, +1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
  { .pos = { -1.0f, +1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 0.0f } },
  { .pos = { -1.0f, -1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { -1.0f, +1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
  
  // Up
  { .pos = { -1.0f, +1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { -1.0f, +1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 1.0f } },
  { .pos = { +1.0f, +1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
  { .pos = { +1.0f, +1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 0.0f } },
  { .pos = { -1.0f, +1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { +1.0f, +1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
  
  // Down
  { .pos = { -1.0f, -1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 1.0f } },
  { .pos = { -1.0f, -1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { +1.0f, -1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
  { .pos = { -1.0f, -1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { +1.0f, -1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 0.0f } },
  { .pos = { +1.0f, -1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
  
  // Front
  { .pos = { -1.0f, -1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 1.0f } },
  { .pos = { -1.0f, +1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { +1.0f, -1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
  { .pos = { -1.0f, +1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { +1.0f, +1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 0.0f } },
  { .pos = { +1.0f, -1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
  
  // Back
  { .pos = { -1.0f, +1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { -1.0f, -1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 1.0f } },
  { .pos = { +1.0f, -1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
  { .pos = { +1.0f, +1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 0.0f } },
  { .pos = { -1.0f, +1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { +1.0f, -1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
};

static const int num_cube_vertices = sizeof(cube_vertices) / sizeof(vertex_t);

bool skybox_init(skybox_t *skybox, vertex_buffer_t *vertex_buffer)
{
  glGenTextures(1, &skybox->texture);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->texture);
  
  static const char *faces[] = {
    "res/skybox/night/right.jpg",
    "res/skybox/night/left.jpg",
    "res/skybox/night/up.jpg",
    "res/skybox/night/down.jpg",
    "res/skybox/night/back.jpg",
    "res/skybox/night/front.jpg",
  };
  
  for (int i = 0; i < 6; i++) {
    SDL_Surface *bitmap = IMG_Load(faces[i]);
    
    if (!bitmap) {
      LOG_ERROR("could not load %s", faces[i]);
      return false;
    }

#ifdef __EMSCRIPTEN__
    glTexImage2D(
      GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,
      bitmap->w, bitmap->h,
      0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap->pixels
    );
#else
    glTexImage2D(
      GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
      bitmap->w, bitmap->h,
      0, GL_RGB, GL_UNSIGNED_BYTE, bitmap->pixels
    );
#endif
    
    SDL_FreeSurface(bitmap);
  }
  
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  
  char *src_vertex = file_read_all("res/shader/skybox.vert");
  char *src_fragment = file_read_all("res/shader/skybox.frag");

  if (!shader_load(&skybox->shader, "", src_vertex, src_fragment)) {
    LOG_ERROR("failed to load shader");
    return false;
  }
  
  free(src_vertex);
  free(src_fragment);
  
  GLuint ul_skybox = glGetUniformLocation(skybox->shader, "u_skybox");
  
  GLuint ubl_matrices = glGetUniformBlockIndex(skybox->shader, "ubo_matrices");
  glUniformBlockBinding(skybox->shader, ubl_matrices, 0);
  
  glUseProgram(skybox->shader);
  glUniform1i(ul_skybox, 0);
  
  if (!vertex_buffer_new_mesh(
    vertex_buffer,
    &skybox->mesh,
    cube_vertices,
    num_cube_vertices
  )) {
    return false;
  }
  
  return true;
}

void skybox_render(skybox_t *skybox, view_t *view, quat_t view_angle)
{
  glDepthMask(GL_FALSE);
  
  glUseProgram(skybox->shader);
  
  view_move(view, vec3_init(0.0f, 0.0f, 0.0f), view_angle);
  view_sub_data(view, mat4x4_init_identity());
  
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->texture);
  
  glDrawArrays(GL_TRIANGLES, skybox->mesh.ptr, skybox->mesh.num_vertices);
  
  glDepthMask(GL_TRUE);
}
