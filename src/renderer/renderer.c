#include "renderer.h"

#include "gl.h"

static void renderer_init_gl();

static bool renderer_init_mesh(renderer_t *r);
static bool renderer_init_texture(renderer_t *r);
static bool renderer_init_material(renderer_t *r);

bool renderer_init(renderer_t *r, const game_t *game)
{
  renderer_init_gl();
  mesh_buffer_init(&r->mesh_buffer, 1024 * 1024);
  
  gls_flat_init(&r->gls_flat);
  
  camera_init(&r->camera);
  camera_perspective(&r->camera, 720.0 / 1280.0, to_radians(90.0), 0.1, 100.0);
  // camera_orthogonal(&r->camera, 720.0 / 1280.0, 10.0, -50, 50);
  
  r->game = game;
  
  if (!model_load(&r->fumo_model, &r->mesh_buffer, "cirno_fumo")) {
    return false;
  }
  
  return true;
}

static void renderer_init_gl()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  // glEnable(GL_CULL_FACE);
  // glCullFace(GL_FRONT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
}

void renderer_render(renderer_t *r)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  gls_flat_bind(&r->gls_flat);
  
  camera_move(&r->camera, r->game->position, r->game->rotation);
  camera_sub_data(&r->camera, mat4x4_init_identity());
  
  model_draw(&r->fumo_model);
  model_draw(&r->map_model);
}

void renderer_map_load(renderer_t *r, map_file_t *map_file)
{
  model_load_map(&r->map_model, &r->mesh_buffer, map_file);
}
