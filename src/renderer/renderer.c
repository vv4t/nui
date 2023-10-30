#include "renderer.h"

#include "gl.h"

#include "camera.h"
#include "model.h"
#include "gls_flat.h"
#include "material.h"

struct renderer_s {
  view_t  view;
  model_t fumo_model;
  model_t map_model;
};

static void renderer_init_gl();

static bool renderer_init_mesh(renderer_t *r);
static bool renderer_init_texture(renderer_t *r);
static bool renderer_init_material(renderer_t *r);

bool renderer_init(renderer_t *r, const game_t *game)
{
  renderer_init_gl();
  mesh_buffer_init(1024 * 1024);
  
  flat_init();
  light_init();
  camera_init();
  
  view_init_perspective(&r.view, to_radians(90.0), 720.0, 1280.0, 0.1, 100.0);
  camera_set_view(r.view);
  
  r->game = game;
  
  if (!model_load(&r->fumo_model, "cirno_fumo")) {
    return false;
  }
  
  return true;
}

static void renderer_init_gl()
{
  glClearColor(0.6f, 0.6f, 1.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
}

void renderer_render()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  flat_bind();
  
  camera_move(r.game->player.position, r.game->player.rotation);
  camera_sub_data(mat4x4_init_identity());
  
  model_draw(&r.fumo_model);
  model_draw(&r.map_model);
}

void renderer_map_load(const map_t *map)
{
  model_load_map(&r->map_model, map);
}
