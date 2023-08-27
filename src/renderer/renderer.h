#ifndef RENDERER_H
#define RENDERER_H

#include "renderer_api.h"

#include "camera.h"
#include "buffer.h"
#include "gls_flat.h"
#include "material.h"
#include "../game/game.h"

struct renderer_s {
  const game_t  *game;
  
  buffer_t      buffer;
  camera_t      camera;
  
  mesh_t        scene_mesh;
  mesh_t        quad_mesh;
  
  gls_flat_t    gls_flat;
  
  GLuint        tile_diffuse;
  material_t    tile_mtl;
};

bool renderer_init(renderer_t *r, const game_t *game);
void renderer_render(renderer_t *r);

#endif
