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
  
  skybox_t        skybox;
  flat_t          flat;
  
  scene_t         scene;
  
  material_t      tile_mtl;
  
  mesh_t          scene_mesh;
  mesh_t          cube_mesh;
} renderer_t;

bool renderer_init(renderer_t *renderer);
void renderer_render(renderer_t *renderer, const game_t *game);

#endif
