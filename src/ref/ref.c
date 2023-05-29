#include "ref.h"

#include "gl.h"
#include "../ref_pipes/ref_pipe_main.h"

static void ref_init_gl(ref_t *ref);
static bool ref_init_scene(ref_t *ref);
static bool ref_init_shaders(ref_t *ref);

static bool ref_init_material(ref_t *ref);
static bool ref_init_texture(ref_t *ref);
static bool ref_init_mesh(ref_t *ref);

static void ref_render_scene(ref_t *ref, const game_t *game);

static void ref_light_pass(void *data, mat4x4_t light_matrix);

bool ref_init(ref_t *ref)
{
  buffer_init(&ref->buffer, 4096);
  
  view_init(&ref->view);
  ref->view.projection_matrix = mat4x4_init_perspective(720.0 / 1280.0, to_radians(90.0), 0.1, 100.0);
  
  vertex_t quad_vertices[] = {
    { .pos = { -1.0f, -1.0f, 0.0f }, .uv = { 0.0f, 0.0f } },
    { .pos = { -1.0f, +1.0f, 0.0f }, .uv = { 0.0f, 1.0f } },
    { .pos = { +1.0f, -1.0f, 0.0f }, .uv = { 1.0f, 0.0f } },
    { .pos = { -1.0f, +1.0f, 0.0f }, .uv = { 0.0f, 1.0f } },
    { .pos = { +1.0f, +1.0f, 0.0f }, .uv = { 1.0f, 1.0f } },
    { .pos = { +1.0f, -1.0f, 0.0f }, .uv = { 1.0f, 0.0f } }
  };

  if (!buffer_new_mesh(
    &ref->buffer,
    &ref->quad_mesh,
    quad_vertices,
    6
  )) {
    return false;
  }
  
  if (!lights_init(&ref->lights))
    return false;
  
  if (!hdr_init(&ref->hdr, ref->quad_mesh))
    return false;
  
  if (!skybox_init(&ref->skybox, &ref->buffer))
    return false;
  
  ref->ref_pipe = (ref_pipe_t) {
    .buffer = &ref->buffer,
    .hdr    = &ref->hdr,
    .lights = &ref->lights,
    .skybox = &ref->skybox,
    .init_scene = ref_pipe_main_init_scene,
    .render_scene = ref_pipe_main_render_scene
  };
  
  if (!ref->ref_pipe.init_scene(&ref->ref_pipe, &ref->scene, &ref->view))
    return false;
  
  return true;
}

void ref_render(ref_t *ref, const game_t *game)
{
  ref->ref_pipe.render_scene(&ref->ref_pipe, &ref->scene, game, &ref->view);
}
