#include "skybox.h"

#include "log.h"
#include "file.h"
#include "shader.h"
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
    "res/skybox/right.png",
    "res/skybox/left.png",
    "res/skybox/up.png",
    "res/skybox/down.png",
    "res/skybox/back.png",
    "res/skybox/front.png",
  };
  
  for (int i = 0; i < 6; i++) {
    SDL_Surface *bitmap = IMG_Load(faces[i]);
    
    if (!bitmap) {
      LOG_ERROR("could not load %s", faces[i]);
      return false;
    }
    
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, bitmap->w, bitmap->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap->pixels);
    
    SDL_FreeSurface(bitmap);
  }
  
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  
  char *src_vertex = file_read_all("res/shader/skybox.vert");
  char *src_fragment = file_read_all("res/shader/skybox.frag");

  if (!shader_load(&skybox->shader, src_vertex, NULL, src_fragment)) {
    LOG_ERROR("failed to load shader");
    return false;
  }
  
  free(src_vertex);
  free(src_fragment);
  
  GLuint ul_skybox = glGetUniformLocation(skybox->shader, "u_skybox");
  skybox->ul_mvp = glGetUniformLocation(skybox->shader, "u_mvp");
  
  glUseProgram(skybox->shader);
  glUniform1i(ul_skybox, 2);
  
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

void skybox_render(skybox_t *skybox, mat4x4_t projection_matrix, quat_t view_angle)
{
  glDepthMask(GL_FALSE);
  
  glUseProgram(skybox->shader);
  
  quat_t inverse_view_angle = quat_conjugate(view_angle);
  mat4x4_t rotation_matrix = mat4x4_init_rotation(inverse_view_angle);
  mat4x4_t view_projection_matrix = mat4x4_mul(rotation_matrix, projection_matrix);
  
  glUniformMatrix4fv(skybox->ul_mvp, 1, GL_FALSE, view_projection_matrix.m);
  
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->texture);
  
  glDrawArrays(GL_TRIANGLES, skybox->mesh.ptr, skybox->mesh.num_vertices);
  
  glDepthMask(GL_TRUE);
}
