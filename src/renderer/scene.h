#ifndef PIPELINE_H
#define PIPELINE_H

#include "renderer_def.h"
#include "waves.h"
#include "view.h"
#include "skybox.h"
#include "lights.h"
#include "hdr.h"
#include "../game/game.h"

typedef struct scene {
  buffer_t    *buffer;
  
  hdr_t       *hdr;
  skybox_t    *skybox;
  lights_t    *lights;
  waves_t     *waves;
  
  mesh_t      meshes[MAX_MESHES];
  texture_t   textures[MAX_TEXTURES];
  material_t  materials[MAX_MATERIALS];
  
  bool (*init)(struct scene *scene, view_t *view);
  void (*render)(struct scene *scene, const game_t *game, view_t *view);
  void (*draw)(struct scene *scene, const view_t *view);
} scene_t;

#endif
