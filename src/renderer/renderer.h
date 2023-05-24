#ifndef RENDERER_H
#define RENDERER_H

#include "view.h"
#include "scene.h"
#include "vertex_buffer.h"

#include "../shaders/skybox.h"
#include "../shaders/hdr.h"
#include "../shaders/lights.h"
#include "../shaders/colors.h"
#include "../shaders/waves.h"
#include "../shaders/flat.h"

#include "../game/game.h"

typedef struct {
  vertex_buffer_t vertex_buffer;
  view_t          view;
  
  hdr_t           hdr;
  skybox_t        skybox;
  lights_t        lights;
  colors_t        colors;
  waves_t         waves;
  flat_t   flat;
  
  scene_t         scene;
  
  light_t         light1;
  light_t         light2;
  
  material_t      stone_mtl;
  material_t      tile_mtl;
  material_t      water_mtl;
  
  mesh_t          quad_mesh;
  mesh_t          scene_mesh;
  mesh_t          cube_mesh;
} renderer_t;

bool renderer_init(renderer_t *renderer);
void renderer_render(renderer_t *renderer, const game_t *game);

#endif
