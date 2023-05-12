#ifndef RENDERER_H
#define RENDERER_H

#include "view.h"
#include "scene.h"
#include "skybox.h"
#include "vertex_buffer.h"

#include "../shaders/hdr.h"
#include "../shaders/lights.h"
#include "../shaders/colors.h"

#include "../game/game.h"

typedef struct {
  vertex_buffer_t vertex_buffer;
  view_t          view;
  
  hdr_t           hdr;
  skybox_t        skybox;
  lights_t        lights;
  colors_t        colors;
  
  scene_t         scene;
  
  material_t      mtl_ground;
  material_t      mtl_tile;
  mesh_t          scene_mesh;
  mesh_t          cube_mesh;
} renderer_t;

bool renderer_init(renderer_t *renderer);
void renderer_render(renderer_t *renderer, const game_t *game);

#endif
