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

static void renderer_game_draw(
  void      *data,
  GLuint    ubo_matrices,
  mat4x4_t  view_projection_matrix,
  vec3_t    view_pos);

bool renderer_init(renderer_t *renderer)
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  
  if (!renderer_init_mesh(renderer))
    return false;
  
  if (!renderer_init_material(renderer))
    return false;
  
  if (!skybox_init(&renderer->skybox, &renderer->vertex_buffer))
    return false;
  
  if (!lights_init(&renderer->lights))
    return false;
  
  if (!colors_init(&renderer->colors))
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
    2,
    &renderer->scene_draw,
    vec3_init(5.0, 5.0, 5.0),
    10.0,
    vec4_init(0.0, 1.0, 0.5, 1.0)
  );
  
  return true;
}

void renderer_render(renderer_t *renderer, const game_t *game)
{
  vec3_t light_pos_0 = vec3_init(
    cos(2 * game->time) * 1.0,
    cos(0.5 * game->time) * 4.0,
    sin(2 * game->time) * 1.0
  );
  
  lights_set_light(
    &renderer->lights,
    0,
    &renderer->scene_draw,
    light_pos_0,
    10.0,
    vec4_init(0.0, 1.0, 1.0, 1.0)
  );
  
  vec3_t light_pos_1 = vec3_init(
    0.0,
    sin(0.5 * game->time) * 4.0,
    0.0 
  );
  
  lights_set_light(
    &renderer->lights,
    1,
    &renderer->scene_draw,
    light_pos_1,
    10.0,
    vec4_init(1.0, 0.0, 1.0, 1.0)
  );
  
  glViewport(0, 0, 1280, 720);
  glClear(GL_DEPTH_BUFFER_BIT);
  
  renderer_setup_view_projection_matrix(renderer, game);
  skybox_render(&renderer->skybox, renderer->projection_matrix, game->rotation);
  renderer_game_render(renderer, game);
  
  colors_bind(&renderer->colors);
  
  set_matrices(
    renderer->ubo_matrices,
    mat4x4_init_translation(light_pos_0),
    renderer->view_projection_matrix,
    game->position);
  colors_set_color(&renderer->colors, vec4_init(0.9, 1.0, 1.0, 1.0));
  draw_mesh(renderer->cube_mesh);
  
  set_matrices(
    renderer->ubo_matrices,
    mat4x4_init_translation(light_pos_1),
    renderer->view_projection_matrix,
    game->position);
  colors_set_color(&renderer->colors, vec4_init(1.0, 0.9, 1.0, 1.0));
  draw_mesh(renderer->cube_mesh);
  
  set_matrices(
    renderer->ubo_matrices,
    mat4x4_init_translation(vec3_init(5.0, 5.0, 5.0)),
    renderer->view_projection_matrix,
    game->position);
  colors_set_color(&renderer->colors, vec4_init(1.0, 1.0, 1.0, 1.0));
  draw_mesh(renderer->cube_mesh);
}

static void renderer_game_draw(
  void      *data,
  GLuint    ubo_matrices,
  mat4x4_t  view_projection_matrix,
  vec3_t    view_pos)
{
  set_matrices(
    ubo_matrices,
    mat4x4_init_identity(),
    view_projection_matrix,
    view_pos);
  
  draw_mesh((*(renderer_t*) data).scene_mesh);
}

static void renderer_game_render(renderer_t *renderer, const game_t *game)
{
  lights_bind(&renderer->lights);
  
  set_matrices(
    renderer->ubo_matrices,
    mat4x4_init_identity(),
    renderer->view_projection_matrix,
    game->position);
  
  lights_bind_material(&renderer->mtl_ground);
  draw_mesh(renderer->scene_mesh);
}

static void renderer_setup_view_projection_matrix(renderer_t *renderer, const game_t *game)
{
  /*
  vec3_t view_origin = vec3_mulf(game->position, -1);
  quat_t view_rotation = quat_conjugate(game->rotation);
  
  mat4x4_t translation_matrix = mat4x4_init_translation(view_origin);
  mat4x4_t rotation_matrix = mat4x4_init_rotation(view_rotation);
  
  mat4x4_t view_matrix = mat4x4_mul(translation_matrix, rotation_matrix);
  
  renderer->view_projection_matrix = mat4x4_mul(view_matrix, renderer->projection_matrix);
  */
  
  vec3_t view_pos = vec3_init(cos(0.5 * game->time) * 6.9, 2.0 + cos(game->time), sin(0.5 * game->time) * 6.9);
  
  mat4x4_t view_matrix = mat4x4_init_look_at(
    vec3_init(0.0, 2.0, 0.0),
    view_pos,
    vec3_init(0.0, 1.0, 0.0)
  );
  
  renderer->view_projection_matrix = mat4x4_mul(view_matrix, renderer->projection_matrix);
}

static bool renderer_init_material(renderer_t *renderer)
{
  if (!material_load(
    &renderer->mtl_ground,
    "res/texture/tile/tile_color.jpg",
    "res/texture/tile/tile_normal.jpg")
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
  
  /*--- scene.mesh ---*/
  
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
  
  /*--- cube.mesh ---*/
  
  if (!mesh_file_load(&mesh_file, "res/mesh/cube.mesh"))
    return false;
  
  if (!vertex_buffer_new_mesh(
    &renderer->vertex_buffer,
    &renderer->cube_mesh,
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
