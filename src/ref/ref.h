#ifndef REF_H
#define REF_H

#include "view.h"
#include "buffer.h"
#include "ref_def.h"
#include "ref_pipe.h"

#include "../shaders/skybox.h"
#include "../shaders/hdr.h"
#include "../shaders/lights.h"
#include "../shaders/colors.h"
#include "../shaders/waves.h"
#include "../shaders/flat.h"

#include "../game/game.h"

typedef struct {
  buffer_t    buffer;
  view_t      view;
  
  skybox_t    skybox;
  lights_t    lights;
  
  scene_t     scene;
  ref_pipe_t  ref_pipe;
} ref_t;

bool ref_init(ref_t *ref);
void ref_render(ref_t *ref, const game_t *game);

#endif
