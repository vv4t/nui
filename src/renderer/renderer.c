#include "renderer.h"

#include "gl.h"
#include "mesh_file.h"

static bool renderer_init_texture(renderer_t *r);
static bool renderer_init_mesh(renderer_t *r);

static bool load_mesh_file(buffer_t *buffer, mesh_t *mesh, const char *path);

bool renderer_init(renderer_t *r)
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
  
  buffer_init(&r->buffer, 4096);
  
  camera_init(&r->camera);
  camera_perspective(&r->camera, to_radians(90.0), 720.0 / 1280.0, 0.1, 100.0);
  
  if (!renderer_init_mesh(r)) {
    return false;
  }
  
  if (!renderer_init_texture(r)) {
    return false;
  }
  
  return true;
}

void renderer_render(renderer_t *r)
{
  camera_move(&r->camera, r->game->position, r->game->rotation);
  
  camera_sub_data(&r->camera, mat4x4_init_identity());
  mesh_draw(r->scene_mesh);
}

static bool renderer_init_mesh(renderer_t *r)
{
  if (!load_mesh_file(&r->buffer, &r->scene_mesh, "res/mesh/scene.mesh")) {
    return false;
  }
  
  return true;
}


static bool load_mesh_file(buffer_t *buffer, mesh_t *mesh, const char *path)
{
  mesh_file_t mesh_file;
  
  if (!mesh_file_load(&mesh_file, path)) {
    return false;
  }
  
  if (!buffer_new_mesh(buffer, mesh, mesh_file.vertices, mesh_file.num_vertices)) {
    return false;
  }
  
  mesh_file_free(&mesh_file);
  
  return true;
}

static bool renderer_init_texture(renderer_t *r)
{
  if (!texture_load(&r->tile_diffuse, "res/mtl/tile/color.jpg")) {
    return false;
  }
  
  return true;
}
