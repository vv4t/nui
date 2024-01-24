#include "renderer.h"

#include "light.h"
#include "wave.h"
#include "camera.h"
#include "model.h"
#include "material.h"
#include "api.h"
#include "frame.h"
#include "defer.h"
#include "../pipeline/pipeline.h"
#include "../gl/gl.h"
#include "../gl/quad.h"
#include "../gl/mesh.h"
#include "../gl/shader.h"
#include "../ngui/ngui.h"
#include "../common/log.h"

typedef struct {
  view_t view;
  model_t map_model;
  pipeline_t pipeline;
  const game_t *game;
} renderer_t;

static renderer_t renderer;

static void renderer_init_gl();
static void renderer_scene_render();

bool renderer_init(const game_t *game)
{
  renderer.game = game;
  
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_SCISSOR_TEST);
  
  mesh_buffer_init(100000);
  material_init();
  camera_init();
  
  if (!quad_init()) {
    return false;
  }
  
  if (!light_init()) {
    return false;
  }
  
  if (!wave_init()) {
    return false;
  }
  
  
  if (!defer_init(VIEW_WIDTH, VIEW_HEIGHT)) {
    return false;
  }
  
  view_set_perspective(&renderer.view, (float) SCR_HEIGHT/ (float) SCR_WIDTH, to_radians(90.0), 0.1, 100.0);
  
  renderer.pipeline = pipeline_reflect;
  
  if (!renderer.pipeline.init()) {
    return false;
  }
  
  return true;
}

void renderer_render()
{
  if (renderer.game->light_update) {
    light_sub_point(0, renderer.game->light_pos, 6.0, vec3_init(1.0, 0.2, 1.0));
  }
  
  renderer.pipeline.setup();
  
  camera_set_view(renderer.view);
  camera_move(renderer.game->player.position, renderer.game->player.rotation);
  
  renderer.pipeline.pass();
}

void renderer_scene_pass()
{
  camera_model(mat4x4_init_identity());
  model_draw(&renderer.map_model);
}

void renderer_shadow_pass()
{
  glClear(GL_DEPTH_BUFFER_BIT);
  
  camera_model(mat4x4_init_identity());
  model_draw(&renderer.map_model);
}

void renderer_map_load(const map_t *map)
{
  model_load_map(&renderer.map_model, map);
  
  for (int i = 0; i < map->num_lights; i++) {
    light_sub_point(1 + i, map->lights[i].pos, map->lights[i].intensity, map->lights[i].color);
  }
}
