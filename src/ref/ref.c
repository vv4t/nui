#include "ref.h"

#include "gl.h"
#include "../piplines/main_pipeline.h"

static bool ref_init_primitive_mesh(ref_t *ref);

bool ref_init(ref_t *ref)
{
  buffer_init(&ref->buffer, 4096);
  
  view_init(&ref->view);
  ref->view.projection_matrix = mat4x4_init_perspective(720.0 / 1280.0, to_radians(90.0), 0.1, 100.0);
  
  ref_init_primitive_mesh(ref);
  
  if (!lights_init(&ref->lights))
    return false;
  
  if (!hdr_init(&ref->hdr, ref->quad_mesh))
    return false;
  
  if (!skybox_init(&ref->skybox, &ref->buffer))
    return false;
  
  ref->pipeline = (pipeline_t) {
    .buffer = &ref->buffer,
    .hdr    = &ref->hdr,
    .lights = &ref->lights,
    .skybox = &ref->skybox,
    .init_scene = main_pipeline_init_scene,
    .render_scene = main_pipeline_render_scene
  };
  
  if (!ref->pipeline.init_scene(&ref->pipeline, &ref->scene, &ref->view))
    return false;
  
  return true;
}

void ref_render(ref_t *ref, const game_t *game)
{
  ref->pipeline.render_scene(&ref->pipeline, &ref->scene, game, &ref->view);
}

static bool ref_init_primitive_mesh(ref_t *ref)
{
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
}
