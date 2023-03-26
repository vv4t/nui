#include "renderer.h"

#include "gl.h"
#include "shader.h"
#include "file.h"
#include "mesh_file.h"

static bool renderer_init_shadow(renderer_t *renderer);
static bool renderer_init_material(renderer_t *renderer);
static void renderer_init_lights(renderer_t *renderer);
static void renderer_init_matrices(renderer_t *renderer);
static bool renderer_init_texture(renderer_t *renderer);
static bool renderer_init_mesh(renderer_t *renderer);
static void renderer_init_projection_matrix(renderer_t *renderer);

static void renderer_light_set(renderer_t *renderer, int light_id, vec3_t pos, float intensity, vec4_t color);
static void renderer_setup_view_projection_matrix(renderer_t *renderer, const game_t *game);

static void renderer_shadow_render(renderer_t *renderer, const game_t *game);

static void renderer_game_render(renderer_t *renderer, const game_t *game);

bool renderer_init(renderer_t *renderer)
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  
  glCullFace(GL_FRONT);
  
  renderer_init_lights(renderer);
  renderer_init_matrices(renderer);
  
  if (!main_shader_init(&renderer->main_shader))
    return false;
  
  if (!renderer_init_mesh(renderer))
    return false;
  
  if (!skybox_init(&renderer->skybox, &renderer->vertex_buffer))
    return false;
  
  if (!renderer_init_material(renderer))
    return false;
  
  if (!renderer_init_shadow(renderer))
    return false;
  
  char *src_vertex = file_read_all("res/shader/white.vert");
  char *src_fragment = file_read_all("res/shader/white.frag");
  
  if (!shader_load(&renderer->white_shader, src_vertex, NULL, src_fragment))
    return false;
  
  free(src_vertex);
  free(src_fragment);
  
  renderer_init_projection_matrix(renderer);
  renderer_light_set(renderer, 0, vec3_init(0.0, 4.0, -4.0), 50.0, vec4_init(1.0, 1.0, 1.0, 1.0));
  glBindBuffer(GL_UNIFORM_BUFFER, renderer->ubo_matrices);
  material_bind(&renderer->mtl_ground);
  
  return true;
}

void renderer_render(renderer_t *renderer, const game_t *game)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  renderer_setup_view_projection_matrix(renderer, game);
  
  skybox_render(&renderer->skybox, renderer->projection_matrix, game->rotation);
  renderer_shadow_render(renderer, game);
  renderer_game_render(renderer, game);
  
  glUseProgram(renderer->white_shader);
  
  float cos_time = cos(game->time);
  float sin_time = sin(game->time);
  
  vec3_t light_pos = vec3_init(sin_time, cos_time * 3, cos_time);
  
  mat4x4_t model_matrix = mat4x4_init_translation(light_pos);
  mat4x4_t mvp_matrix = mat4x4_mul(model_matrix, renderer->view_projection_matrix);
  
  ubo_matrices_t ubo_matrices = {
    .model = model_matrix,
    .mvp = mvp_matrix,
    .view_pos = game->position
  };
  
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ubo_matrices_t), &ubo_matrices);
  glDrawArrays(GL_TRIANGLES, renderer->cube_mesh.ptr, renderer->cube_mesh.num_vertices);
}

static void renderer_shadow_render(renderer_t *renderer, const game_t *game)
{
  mat4x4_t projection_matrix = mat4x4_init_perspective(1.0, to_radians(90), 0.1, 100.0);
  mat4x4_t rotation_matrices[] = {
    mat4x4_init_rotation(quat_init_rotation(vec3_init(0.0, 1.0, 0.0), to_radians(+90))),  // left
    mat4x4_init_rotation(quat_init_rotation(vec3_init(0.0, 1.0, 0.0), to_radians(-90))),  // right
    
    mat4x4_init_rotation(
      quat_mul(
        quat_init_rotation(vec3_init(1.0, 0.0, 0.0), to_radians(-90)),
        quat_init_rotation(vec3_init(0.0, 1.0, 0.0), to_radians(180))
      )
    ), // down
    
    mat4x4_init_rotation(
      quat_mul(
        quat_init_rotation(vec3_init(1.0, 0.0, 0.0), to_radians(+90)),
        quat_init_rotation(vec3_init(0.0, 1.0, 0.0), to_radians(180))
      )
    ), // up
    
    mat4x4_init_rotation(quat_init_rotation(vec3_init(0.0, 1.0, 0.0), to_radians(180))),  // front
    mat4x4_init_rotation(quat_init_rotation(vec3_init(0.0, 1.0, 0.0), to_radians(0)))     // back
  };
  
  glUseProgram(renderer->shadow_shader);
  
  glViewport(0, 0, 1024, 1024);
  glBindFramebuffer(GL_FRAMEBUFFER, renderer->depth_map_fbo);
  
  float cos_time = cos(game->time);
  float sin_time = sin(game->time);
  
  vec3_t light_pos = vec3_init(sin_time, cos_time * 3, cos_time);
  renderer_light_set(renderer, 0, light_pos, 50.0, vec4_init(cos_time * cos_time, sin_time * sin_time, sin_time * sin_time, 1.0));
  
  mat4x4_t translation_matrix = mat4x4_init_translation(vec3_mulf(light_pos, -1));
  
  for (int i = 0; i < 6; i++) {
    mat4x4_t light_matrix = mat4x4_mul(translation_matrix, rotation_matrices[i]);
    mat4x4_t light_projection_matrix = mat4x4_mul(light_matrix, projection_matrix);
    mat4x4_t model_matrix = mat4x4_init_identity();
    mat4x4_t mvp_matrix = mat4x4_mul(model_matrix, light_projection_matrix);
    
    ubo_matrices_t ubo_matrices = {
      .model = model_matrix,
      .mvp = mvp_matrix,
      .view_pos = light_pos
    };
    
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ubo_matrices_t), &ubo_matrices);
    
    GLuint face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, face, renderer->depth_cube_map, 0);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, renderer->scene_mesh.ptr, renderer->scene_mesh.num_vertices);
  }
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  glViewport(0, 0, 1280, 720);
}

static void renderer_game_render(renderer_t *renderer, const game_t *game)
{
  glUseProgram(renderer->main_shader.program);
  
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_CUBE_MAP, renderer->depth_cube_map);
  
  mat4x4_t model_matrix = mat4x4_init_identity();
  mat4x4_t mvp_matrix = mat4x4_mul(model_matrix, renderer->view_projection_matrix);
  
  ubo_matrices_t ubo_matrices = {
    .model = model_matrix,
    .mvp = mvp_matrix,
    .view_pos = game->position
  };
  
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ubo_matrices_t), &ubo_matrices);
  
  glDrawArrays(GL_TRIANGLES, renderer->scene_mesh.ptr, renderer->scene_mesh.num_vertices);
}

static bool renderer_init_shadow(renderer_t *renderer)
{
  glGenFramebuffers(1, &renderer->depth_map_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, renderer->depth_map_fbo);
  
  glGenTextures(1, &renderer->depth_cube_map);
  glBindTexture(GL_TEXTURE_CUBE_MAP, renderer->depth_cube_map);
  
  for (int i = 0; i < 6; i++) { 
    glTexImage2D(
      GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
      0,
      GL_R32F,
      1024, 1024,
      0,
      GL_RED,
      GL_FLOAT, NULL
    );
  }
  
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  glBindFramebuffer(GL_FRAMEBUFFER, renderer->depth_map_fbo);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderer->depth_cube_map, 0);
  glDrawBuffer(GL_COLOR_ATTACHMENT0);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);  
  
  char *src_vertex = file_read_all("res/shader/shadow.vert");
  char *src_fragment = file_read_all("res/shader/shadow.frag");
  
  if (!shader_load(&renderer->shadow_shader, src_vertex, NULL, src_fragment))
    return false;
  
  free(src_vertex);
  free(src_fragment);
  
  GLuint ubl_matrices = glGetUniformBlockIndex(renderer->shadow_shader, "ubo_matrices");
  glUniformBlockBinding(renderer->shadow_shader, ubl_matrices, 0);
  
  return true;
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
  if (
    !material_init(
      &renderer->mtl_ground,
      "res/texture/ground/ground_color.jpg",
      "res/texture/ground/ground_normal.jpg"
    )
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
    mesh_file.num_vertices
  )) {
    return false;
  }
  mesh_file_free(&mesh_file);
  
  if (!mesh_file_load(&mesh_file, "res/mesh/cube.mesh"))
    return false;
  if (!vertex_buffer_new_mesh(
    &renderer->vertex_buffer,
    &renderer->cube_mesh,
    mesh_file.vertices,
    mesh_file.num_vertices
  )) {
    return false;
  }
  mesh_file_free(&mesh_file);
  
  return true;
}

static void renderer_init_matrices(renderer_t *renderer)
{
  glGenBuffers(1, &renderer->ubo_matrices);
  glBindBuffer(GL_UNIFORM_BUFFER, renderer->ubo_matrices);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(ubo_matrices_t), NULL, GL_DYNAMIC_DRAW);
  
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, renderer->ubo_matrices); 
}

static void renderer_init_lights(renderer_t *renderer)
{
  glGenBuffers(1, &renderer->ubo_lights);
  glBindBuffer(GL_UNIFORM_BUFFER, renderer->ubo_lights);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(ubo_lights_t), NULL, GL_DYNAMIC_DRAW);
  
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, renderer->ubo_lights); 
  
  light_t empty[8] = {0};
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(empty), &empty);
}

static void renderer_light_set(renderer_t *renderer, int light_id, vec3_t pos, float intensity, vec4_t color)
{
  light_t light = {
    .pos = pos,
    .intensity = intensity,
    .color = color
  };
  
  glBindBuffer(GL_UNIFORM_BUFFER, renderer->ubo_lights);
  glBufferSubData(GL_UNIFORM_BUFFER, light_id * sizeof(light_t), sizeof(light_t), &light);
  glBindBuffer(GL_UNIFORM_BUFFER, renderer->ubo_matrices);
}
