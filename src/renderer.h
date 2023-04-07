#ifndef RENDERER_H
#define RENDERER_H

#include "renderer_api.h"

#include "vertex_buffer.h"
#include "game.h"
#include "skybox.h"
#include "lights.h"
#include "colors.h"

typedef struct {
  vertex_buffer_t vertex_buffer;
  GLuint          ubo_matrices;
  
  mat4x4_t        projection_matrix;
  mat4x4_t        view_projection_matrix;
  
  skybox_t        skybox;
  lights_t        lights;
  colors_t        colors;
  
  draw_call_t     scene_draw;
  
  material_t      mtl_ground;
  mesh_t          scene_mesh;
  mesh_t          cube_mesh;
} renderer_t;

bool renderer_init(renderer_t *renderer);
void renderer_render(renderer_t *renderer, const game_t *game);

#endif
