#ifndef WAVE_H
#define WAVE_H

#include "flat.h"
#include "colors.h"

#include "../renderer/gl.h"
#include "../renderer/view.h"
#include "../renderer/renderer_def.h"
#include <stdbool.h>

typedef struct {
  mesh_t  quad_mesh;
  
  shader_t  shader;
  shader_t  out_shader;
  
  texture_t pattern;
  
  texture_t normal_map;
  texture_t wave[2];
  GLuint  fbo[2];
} waves_t;

bool waves_init(waves_t *waves, mesh_t quad_mesh);
void waves_setup(waves_t *waves, flat_t *flat, view_t *view);
void waves_move(waves_t *waves, flat_t *flat, view_t *view);

#endif
