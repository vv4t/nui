#include "renderer.h"

#include "gl.h"
#include "file.h"
#include "mesh_file.h"

static bool renderer_init_material(renderer_t *renderer);
static void renderer_init_matrices(renderer_t *renderer);
static bool renderer_init_texture(renderer_t *renderer);
static bool renderer_init_mesh(renderer_t *renderer);
static void renderer_init_projection_matrix(renderer_t *renderer);

static void renderer_setup_view_projection_matrix(renderer_t *renderer, const game_t *game);
static void renderer_game_render(renderer_t *renderer, const game_t *game);
static void renderer_game_draw(draw_call_t *draw_call, mat4x4_t view_projection_matrix, vec3_t view_pos);

bool renderer_init(renderer_t *renderer)
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  
  if (!renderer_init_mesh(renderer))
    return false;
  
  if (!skybox_init(&renderer->skybox, &renderer->vertex_buffer))
    return false;
  
  if (!lights_init(&renderer->lights))
    return false;
  
  if (!renderer_init_material(renderer))
    return false;
  
  renderer_init_matrices(renderer);
  renderer_init_projection_matrix(renderer);
  
  renderer->scene_draw = (draw_call_t) {
    .ubo_matrices = renderer->ubo_matrices,
    .data = renderer,
    .draw = renderer_game_draw
  };
  
  lights_set_light(
    &renderer->lights,
    0,
    &renderer->scene_draw,
    vec3_init(0.0, 0.0, 0.0),
    40.0,
    vec4_init(1.0, 0.0, 1.0, 1.0)
  );
  
  return true;
}

void renderer_render(renderer_t *renderer, const game_t *game)
{
  glViewport(0, 0, 1280, 720);
  glClear(GL_DEPTH_BUFFER_BIT);
  
  renderer_setup_view_projection_matrix(renderer, game);
  skybox_render(&renderer->skybox, renderer->projection_matrix, game->rotation);
  renderer_game_render(renderer, game);
}

static void renderer_game_draw(draw_call_t *draw_call, mat4x4_t view_projection_matrix, vec3_t view_pos)
{
  mat4x4_t model_matrix = mat4x4_init_identity();
  mat4x4_t mvp_matrix = mat4x4_mul(model_matrix, view_projection_matrix);
  
  ub_matrices_t ub_matrices = {
    .model = model_matrix,
    .mvp = mvp_matrix,
    .view_pos = view_pos
  };
  
  glBindBuffer(GL_UNIFORM_BUFFER, draw_call->ubo_matrices);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ub_matrices_t), &ub_matrices);
  
  draw_mesh((*(renderer_t*) draw_call->data).scene_mesh);
}

static void renderer_game_render(renderer_t *renderer, const game_t *game)
{
  mat4x4_t model_matrix = mat4x4_init_identity();
  mat4x4_t mvp_matrix = mat4x4_mul(model_matrix, renderer->view_projection_matrix);
  
  ub_matrices_t ub_matrices = {
    .model = model_matrix,
    .mvp = mvp_matrix,
    .view_pos = game->position
  };
  
  lights_bind(&renderer->lights);
  
  glBindBuffer(GL_UNIFORM_BUFFER, renderer->ubo_matrices);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ub_matrices_t), &ub_matrices);
  lights_bind_material(&renderer->mtl_ground);
  draw_mesh(renderer->scene_mesh);
}

static void renderer_setup_view_projection_matrix(renderer_t *renderer, const game_t *game)
{
  vec3_t view_origin = vec3_mulf(game->position, -1);
  quat_t view_rotation = quat_conjugate(game->rotation);
  
  mat4x4_t translation_matrix = mat4x4_init_translation(view_origin);
  mat4x4_t rotation_matrix = mat4x4_init_rotation(view_rotation);
  
  mat4x4_t view_matrix = mat4x4_mul(translation_matrix, rotation_matrix);
  
  renderer->view_projection_matrix = mat4x4_mul(view_matrix, renderer->projection_matrix);
}

static bool renderer_init_material(renderer_t *renderer)
{
  if (!material_load(
    &renderer->mtl_ground,
    "res/texture/ground/ground_color.jpg",
    "res/texture/ground/ground_normal.jpg")
  ) {
    return false;
  }
  
  return true;
}

static void renderer_init_projection_matrix(renderer_t *renderer)
{
  renderer->projection_matrix = mat4x4_init_perspective(
    720.0 / 1280.0,
    to_radians(90),
    0.1,
    100.0
  );
}

static bool renderer_init_mesh(renderer_t *renderer)
{
  vertex_buffer_init(&renderer->vertex_buffer, 4096);
  
  mesh_file_t mesh_file;
  
  if (!mesh_file_load(&mesh_file, "res/mesh/scene.mesh"))
    return false;
  
  if (!vertex_buffer_new_mesh(
    &renderer->vertex_buffer,
    &renderer->scene_mesh,
    mesh_file.vertices,
    mesh_file.num_vertices)
  ) {
    return false;
  }
  
  mesh_file_free(&mesh_file);
  
  return true;
}

static void renderer_init_matrices(renderer_t *renderer)
{
  glGenBuffers(1, &renderer->ubo_matrices);
  glBindBuffer(GL_UNIFORM_BUFFER, renderer->ubo_matrices);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(ub_matrices_t), NULL, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, renderer->ubo_matrices); 
}
