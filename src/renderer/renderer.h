#ifndef RENDERER_H
#define RENDERER_H

#include "renderer_api.h"

#include "camera.h"
#include "buffer.h"
#include "../game/game.h"

struct renderer_s {
  const game_t    *game;
  
  buffer_t buffer;
  camera_t        camera;
  
  mesh_t          scene_mesh;
  mesh_t          quad_mesh;
  
  GLuint          tile_diffuse;
};

bool renderer_init(renderer_t *r);
void renderer_render(renderer_t *r);

#endif
