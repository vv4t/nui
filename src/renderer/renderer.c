#include "renderer.h"

#include "gl.h"
#include "mesh_file.h"

static void renderer_init_gl();

static bool renderer_init_mesh(renderer_t *r);
static bool renderer_init_texture(renderer_t *r);
static bool renderer_init_material(renderer_t *r);

static bool load_mesh_file(mesh_buffer_t *mesh_buffer, mesh_t *mesh, const char *path);

bool renderer_init(renderer_t *r, const game_t *game)
{
  renderer_init_gl();
  mesh_buffer_init(&r->mesh_buffer, 4096);
  
  gls_flat_init(&r->gls_flat);
  
  camera_init(&r->camera);
  camera_perspective(&r->camera, 720.0 / 1280.0, to_radians(90.0), 0.1, 100.0);
  // camera_orthogonal(&r->camera, 720.0 / 1280.0, 10.0, -50, 50);
  
  if (!renderer_init_mesh(r)) {
    return false;
  }
  
  if (!renderer_init_texture(r)) {
    return false;
  }
  
  renderer_init_material(r);
  
  r->game = game;
  
  return true;
}

static void renderer_init_gl()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
}

void renderer_render(renderer_t *r)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  gls_flat_bind(&r->gls_flat);
  
  camera_move(&r->camera, r->game->position, r->game->rotation);
  
  material_bind(r->tile_mtl);
  camera_sub_data(&r->camera, mat4x4_init_identity());
  mesh_draw(r->scene_mesh);
}

static bool renderer_init_mesh(renderer_t *r)
{
  if (!load_mesh_file(&r->mesh_buffer, &r->scene_mesh, "res/mesh/scene.mesh")) {
    return false;
  }
  
  return true;
}


static bool load_mesh_file(mesh_buffer_t *mesh_buffer, mesh_t *mesh, const char *path)
{
  mesh_file_t mesh_file;
  
  if (!mesh_file_load(&mesh_file, path)) {
    return false;
  }
  
  if (!mesh_buffer_new(mesh_buffer, mesh, mesh_file.vertices, mesh_file.num_vertices)) {
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

static bool renderer_init_material(renderer_t *r)
{
  r->tile_mtl.diffuse = r->tile_diffuse;
}
