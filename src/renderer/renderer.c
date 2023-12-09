#include "renderer.h"

#include "gl.h"

#define SCR_WIDTH 1280
#define SCR_HEIGHT 720

#define VIEW_SCALE 2

#define VIEW_WIDTH (SCR_WIDTH / VIEW_SCALE)
#define VIEW_HEIGHT (SCR_HEIGHT / VIEW_SCALE)

#include "light.h"
#include "camera.h"
#include "model.h"
#include "flat.h"
#include "material.h"
#include "renderer_api.h"
#include "shader.h"
#include "mesh.h"
#include "frame.h"
#include "hdr.h"
#include "blur.h"

typedef struct {
  view_t view;
  
  model_t fumo_model;
  model_t map_model;
  
  const game_t *game;
} renderer_t;

static renderer_t renderer;

static void renderer_init_gl();
static void renderer_init_scene();

bool renderer_init(const game_t *game)
{
  renderer_init_gl();
  mesh_buffer_init(1024 * 1024);
  
  if (!frame_init()) {
    return false;
  }
  
  if (!flat_init()) {
    return false;
  }
  
  if (!light_init()) {
    return false;
  }
  
  if (!hdr_init(VIEW_WIDTH, VIEW_HEIGHT)) {
    return false;
  }
  
  if (!blur_init(VIEW_WIDTH, VIEW_HEIGHT)) {
    return false;
  }
  
  camera_init();
  
  if (!model_load(&renderer.fumo_model, "cirno_fumo")) {
    return false;
  }
  
  renderer.game = game;
  
  float aspect_ratio = (float) SCR_HEIGHT/ (float) SCR_WIDTH;
  view_set_perspective(&renderer.view, aspect_ratio, to_radians(90.0), 0.1, 100.0);
  
  return true;
}

static void renderer_init_scene()
{
  light_sub_point(0, vec3_init( 3.0, 5.0,  3.0), 30.0, vec4_init(0.0, 1.0, 1.0, 1.0));
  light_sub_point(1, vec3_init(-3.0, 5.0, -3.0), 30.0, vec4_init(1.0, 0.0, 1.0, 1.0));
}

static void renderer_init_gl()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_SCISSOR_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void renderer_render()
{
  /*
  hdr_begin();
  camera_set_view(renderer.view);
  renderer_scene_pass();
  hdr_end();
  
  hdr_draw(0, 0, SCR_WIDTH, SCR_HEIGHT);
  */
  
  blur_begin();
  camera_set_view(renderer.view);
  renderer_scene_pass();
  blur_end();
  
  blur_draw(0, 0, SCR_WIDTH, SCR_HEIGHT);
}

void renderer_scene_pass()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  light_bind();
  light_sub_view_pos(renderer.game->player.position);
  
  camera_move(renderer.game->player.position, renderer.game->player.rotation);
  
  camera_model(mat4x4_init_identity());
  model_draw(&renderer.fumo_model);
  model_draw(&renderer.map_model);
}

void renderer_shadow_pass()
{
  glClear(GL_DEPTH_BUFFER_BIT);
  
  camera_model(mat4x4_init_identity());
  
  model_draw(&renderer.fumo_model);
  model_draw(&renderer.map_model);
}

void renderer_map_load(const map_t *map)
{
  model_load_map(&renderer.map_model, map);
  renderer_init_scene();
}
