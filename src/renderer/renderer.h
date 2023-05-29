#ifndef REF_H
#define REF_H

#include "view.h"
#include "buffer.h"
#include "renderer_def.h"
#include "pipeline.h"
#include "skybox.h"
#include "hdr.h"
#include "lights.h"

#include "../game/game.h"

typedef struct {
  buffer_t    buffer;
  view_t      view;
  
  mesh_t      quad_mesh;
  
  hdr_t       hdr;
  skybox_t    skybox;
  lights_t    lights;
  
  scene_t     scene;
  pipeline_t  pipeline;
} renderer_t;

bool renderer_init(renderer_t *renderer);
void renderer_render(renderer_t *renderer, const game_t *game);

#endif
