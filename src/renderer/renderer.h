#ifndef RENDERER_H
#define RENDERER_H

#include "renderer_api.h"

#include "camera.h"
#include "model.h"
#include "gls_flat.h"
#include "material.h"
#include "../game/game.h"
#include "../game/map.h"

struct renderer_s {
  const game_t  *game;
  
  mesh_buffer_t mesh_buffer;
  camera_t      camera;
  gls_flat_t    gls_flat;
  
  model_t       fumo_model;
  model_t       map_model;
};

bool renderer_init(renderer_t *r, const game_t *game);
void renderer_map_load(renderer_t *r, map_t *map);
void renderer_render(renderer_t *r);

#endif
