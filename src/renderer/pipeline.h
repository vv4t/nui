#ifndef PIPELINE_H
#define PIPELINE_H

#include "renderer_def.h"
#include "waves.h"
#include "view.h"
#include "skybox.h"
#include "lights.h"
#include "hdr.h"
#include "../game/game.h"

typedef struct pipeline {
  buffer_t  *buffer;
  
  hdr_t     *hdr;
  skybox_t  *skybox;
  lights_t  *lights;
  waves_t   *waves;
  
  bool (*init_scene)(struct pipeline *pipeline, scene_t *scene, view_t *view);
  void (*render_scene)(struct pipeline *pipeline, const scene_t *scene, const game_t *game, view_t *view);
} pipeline_t;

#endif
