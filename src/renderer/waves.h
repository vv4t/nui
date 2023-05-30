#ifndef WAVE_H
#define WAVE_H

#include "../renderer/gl.h"
#include "../renderer/view.h"
#include "../renderer/renderer_def.h"
#include <stdbool.h>

typedef struct {
  mesh_t    quad_mesh;
  
  shader_t  shader;
  shader_t  out_shader;
  GLuint    ul_compute;
  
  texture_t pattern;
  
  texture_t normal_map;
  texture_t wave[2];
  GLuint    fbo[2];
} waves_t;

bool waves_init(waves_t *waves, mesh_t quad_mesh);
void waves_move(waves_t *waves);
void waves_setup(waves_t *waves);

#endif
