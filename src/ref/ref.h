#ifndef REF_H
#define REF_H

#include "view.h"
#include "buffer.h"
#include "ref_def.h"

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
  
  texture_t   tile_diffuse_tex;
  texture_t   tile_normal_tex;
  material_t  tile_mtl;
  mesh_t      scene_mesh;
  mesh_t      cube_mesh;
} ref_t;

bool ref_init(ref_t *ref);
void ref_render(ref_t *ref, const game_t *game);

#endif
