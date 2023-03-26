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
  vec3_t  pos;
  float   intensity;
  vec4_t  color;
} light_t;

typedef struct {
  light_t lights[8];
} ubo_lights_t;

typedef struct {
  vertex_buffer_t vertex_buffer;
  
  main_shader_t   main_shader;
  
  mat4x4_t        projection_matrix;
  mat4x4_t        view_projection_matrix;
  
  skybox_t        skybox;
  
  material_t      mtl_ground;
  
  GLuint          ubo_matrices;
  GLuint          ubo_lights;
  
  GLuint          shadow_shader;
  GLuint          depth_map_fbo;
  GLuint          depth_cube_map;
  
  GLuint          white_shader;
  
  mesh_t          scene_mesh;
  mesh_t          cube_mesh;
} renderer_t;

bool renderer_init(renderer_t *renderer);
void renderer_render(renderer_t *renderer, const game_t *game);

#endif
