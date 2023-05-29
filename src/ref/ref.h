#ifndef REF_H
#define REF_H

#include "view.h"
#include "buffer.h"
#include "ref_def.h"
#include "ref_pipe.h"
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
  ref_pipe_t  ref_pipe;
} ref_t;

bool ref_init(ref_t *ref);
void ref_render(ref_t *ref, const game_t *game);

#endif
