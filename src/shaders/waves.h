#ifndef WAVE_H
#define WAVE_H

#include "flat.h"
#include "colors.h"

#include "../renderer/gl.h"
#include "../renderer/view.h"
#include "../renderer/scene.h"
#include <stdbool.h>

typedef struct {
  mesh_t  quad_mesh;
  
  GLuint  shader;
  GLuint  out_shader;
  
  GLuint  pattern;
  GLuint  normal_map;
  
  GLuint  wave[2];
  GLuint  fbo[2];
} waves_t;

bool waves_init(waves_t *waves, mesh_t quad_mesh);
void waves_setup(waves_t *waves, flat_t *flat, view_t *view);
void waves_show(waves_t *waves, flat_t *flat, view_t *view);
void waves_move(waves_t *waves, flat_t *flat, view_t *view);

#endif
