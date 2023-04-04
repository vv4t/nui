#ifndef RENDERER_H
#define RENDERER_H

#include "mesh.h"
#include "game.h"
#include "material.h"
#include "skybox.h"
#include "main_shader.h"

typedef struct {
  mat4x4_t  mvp;
  mat4x4_t  model;
  vec3_t    view_pos;
  float     pad[1];
} ubo_matrices_t;

typedef struct {
  vertex_buffer_t vertex_buffer;
  
  mat4x4_t        projection_matrix;
  mat4x4_t        view_projection_matrix;
  
  skybox_t        skybox;
  lighting_t      lighting;
  
  material_t      mtl_ground;
  
  GLuint          ubo_matrices;
  
  mesh_t          scene_mesh;
} renderer_t;

bool renderer_init(renderer_t *renderer);
void renderer_render(renderer_t *renderer, const game_t *game);

#endif
