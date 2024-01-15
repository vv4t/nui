#include "renderer.h"

#include "light.h"
#include "camera.h"
#include "model.h"
#include "material.h"
#include "api.h"
#include "frame.h"
#include "defer.h"
#include "pipeline1.h"
#include "../gl/gl.h"
#include "../gl/quad.h"
#include "../gl/mesh.h"
#include "../gl/shader.h"
#include "../ngui/ngui.h"
#include "../common/log.h"

typedef struct {
  GLuint shader;
  
  view_t view;
  
  GLuint dither;
  GLuint hdr;
  
  model_t fumo_model;
  model_t map_model;
  
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
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
  
  mesh_buffer_init(10000);
  material_init();
  frame_init(VIEW_WIDTH, VIEW_HEIGHT);
  camera_init();
  
  if (!quad_init()) {
    return false;
  }
  
  if (!light_init()) {
    return false;
  }
  
  if (!defer_init(VIEW_WIDTH, VIEW_HEIGHT)) {
    return false;
  }
  
  if (!model_load(&renderer.fumo_model, "venus")) {
    return false;
  }
  
  view_set_perspective(&renderer.view, (float) SCR_HEIGHT/ (float) SCR_WIDTH, to_radians(90.0), 0.1, 100.0);
  
  if (!pipeline1_init()) {
    return false;
  }
  
  return true;
}

void renderer_render()
{
  if (renderer.game->light_update) {
    light_sub_point(0, renderer.game->light_pos, 6.0, vec3_init(1.0, 0.2, 1.0));
  }
  
  /*
  defer_begin();
  camera_set_view(renderer.view);
  camera_move(renderer.game->player.position, renderer.game->player.rotation);
  renderer_scene_pass();
  defer_end();
  
  frame_begin(0);
  defer_bind();
  light_bind();
  quad_draw();
  frame_end();
  
  frame_begin(1);
  frame_draw(renderer.dither, 0);
  frame_end();
  
  glEnable(GL_BLEND);
  
  camera_set_viewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glClear(GL_DEPTH_BUFFER_BIT);
  frame_draw(renderer.hdr, 1);
  ngui_render();
  
  glDisable(GL_BLEND);
  */
  
  camera_set_view(renderer.view);
  camera_move(renderer.game->player.position, renderer.game->player.rotation);
  
  pipeline1_pass();
}

void renderer_scene_pass()
{
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
  
  for (int i = 0; i < map->num_lights; i++) {
    light_sub_point(1 + i, map->lights[i].pos, map->lights[i].intensity, map->lights[i].color);
  }
}
