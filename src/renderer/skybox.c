#include "skybox.h"

#include "camera.h"
#include "../gl/gl.h"
#include "../gl/mesh.h"
#include "../gl/shader.h"
#include "../common/log.h"
#include "../common/file.h"
#include "../common/path.h"
#include <SDL2/SDL_image.h>

typedef struct {
  texture_t texture;
  shader_t  shader;
  mesh_t    mesh;
} skybox_t;

static skybox_t skybox;

static bool skybox_shader_init();
static bool skybox_mesh_init();
static bool skybox_texture_init(const char *name);

bool skybox_init(const char *name)
{
  if (!skybox_mesh_init()) {
    return false;
  }
  
  if (!skybox_shader_init()) {
    return false;
  }
  
  if (!skybox_texture_init(name)) {
    return false;
  }
  
  return true;
}

static bool skybox_shader_init()
{
  shader_setup_t shader_setup;
  shader_setup_init(&shader_setup, "skybox");
  shader_setup_import(&shader_setup, SHADER_BOTH, "camera");
  
  if (!shader_setup_source(&shader_setup, "assets/shader/skybox", "skybox")) {
    return false;
  }
  
  if (!shader_setup_compile(&skybox.shader, &shader_setup)) {
    return false;
  }
  
  camera_shader_setup(skybox.shader);
  
  glUseProgram(skybox.shader);
  
  GLuint ul_skybox = glGetUniformLocation(skybox.shader, "u_skybox");
  glUniform1i(ul_skybox, 0);
  
  shader_setup_free(&shader_setup);
  
  return true;
}

static bool skybox_texture_init(const char *name)
{
  path_t faces[6];
  path_create(faces[0], "assets/skybox/%s/right.jpg", name);
  path_create(faces[1], "assets/skybox/%s/left.jpg", name);
  path_create(faces[2], "assets/skybox/%s/up.jpg", name);
  path_create(faces[3], "assets/skybox/%s/down.jpg", name);
  path_create(faces[4], "assets/skybox/%s/back.jpg", name);
  path_create(faces[5], "assets/skybox/%s/front.jpg", name);
  
  glGenTextures(1, &skybox.texture);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.texture);
  
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
  
  return true;
}

static bool skybox_mesh_init()
{
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
  
  if (!mesh_buffer_new(&skybox.mesh, cube_vertices, num_cube_vertices)) {
    return false;
  }
  
  return true;
}

void skybox_render()
{
  glDepthMask(GL_FALSE);
  
  glUseProgram(skybox.shader);
  
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.texture);
  
  camera_model(mat4x4_init_identity());
  mesh_draw(skybox.mesh);
  
  glDepthMask(GL_TRUE);
}

GLuint skybox_get_texture()
{
  return skybox.texture;
}
