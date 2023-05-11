#include "renderer.h"

#include "gl.h"
#include "mesh_file.h"
#include "../common/file.h"

static void renderer_init_gl(renderer_t *renderer);
static bool renderer_init_scene(renderer_t *renderer);
static bool renderer_init_shaders(renderer_t *renderer);

static bool renderer_init_material(renderer_t *renderer);
static bool renderer_init_texture(renderer_t *renderer);
static bool renderer_init_mesh(renderer_t *renderer);
static void renderer_init_lights(renderer_t *renderer);

static void renderer_draw_scene(void *data, view_t *view);

bool renderer_init(renderer_t *renderer)
{
  renderer_init_gl(renderer);
  
  if (!renderer_init_shaders(renderer))
    return false;
  
  if (!renderer_init_scene(renderer))
    return false;
  
  return true;
}

static void renderer_init_gl(renderer_t *renderer)
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  
  view_init(&renderer->view);
  view_perspective(&renderer->view, 720.0 / 1280.0, to_radians(90.0), 0.1, 100.0);
}

static bool renderer_init_shaders(renderer_t *renderer)
{
  if (!lights_init(&renderer->lights))
    return false;
  
  if (!colors_init(&renderer->colors))
    return false;
  
  return true;
}

static bool renderer_init_scene(renderer_t *renderer)
{
  if (!renderer_init_mesh(renderer))
    return false;
  
  if (!renderer_init_material(renderer))
    return false;
  
  if (!skybox_init(&renderer->skybox, &renderer->vertex_buffer))
    return false;
  
  renderer->scene = (scene_t) {
    .data = renderer,
    .view = &renderer->view,
    .draw = renderer_draw_scene
  };
  
  lights_set_light(
    &renderer->lights, 0,
    &renderer->scene,
    vec3_init(5.0, 5.0, 5.0), 10.0, vec4_init(0.0, 1.0, 0.5, 1.0)
  );
  
  lights_set_light(
    &renderer->lights,
    1,
    &renderer->scene,
    vec3_init(-5.0, 5.0, 5.0), 10.0, vec4_init(0.0, 1.0, 1.0, 1.0)
  );
  
  lights_set_light(
    &renderer->lights,
    2,
    &renderer->scene,
    vec3_init(5.0, -5.0, 5.0), 10.0, vec4_init(1.0, 0.0, 1.0, 1.0)
  );
  
  return true;
}

void renderer_render(renderer_t *renderer, const game_t *game)
{
  glViewport(0, 0, 1280, 720);
  glClear(GL_DEPTH_BUFFER_BIT);
  
  skybox_render(&renderer->skybox, &renderer->view, game->rotation);
  
  lights_bind(&renderer->lights);
  view_move(&renderer->view, game->position, game->rotation);
  
  view_sub_data(&renderer->view, mat4x4_init_identity());
  lights_bind_material(&renderer->mtl_tile);
  draw_mesh(renderer->scene_mesh);
}

static void renderer_draw_scene(void *data, view_t *view)
{
  view_sub_data(view, mat4x4_init_identity());
  draw_mesh((*(renderer_t*) data).scene_mesh);
}

static bool renderer_init_material(renderer_t *renderer)
{
  if (
    !material_load(
      &renderer->mtl_ground,
      "res/texture/ground/ground_color.jpg",
      "res/texture/ground/ground_normal.jpg"
    )
  ) {
    return false;
  }
  
  if (
    !material_load(
      &renderer->mtl_tile,
      "res/texture/tile/tile_color.jpg",
      "res/texture/tile/tile_normal.jpg"
    )
  ) {
    return false;
  }
  
  return true;
}

static bool renderer_init_mesh(renderer_t *renderer)
{
  vertex_buffer_init(&renderer->vertex_buffer, 4096);
  
  struct {
    const char  *path;
    mesh_t      *mesh;
  } meshes[] = {
    { .path = "res/mesh/scene.mesh", &renderer->scene_mesh },
    { .path = "res/mesh/cube.mesh", &renderer->cube_mesh },
  };
  
  for (int i = 0; i < 2; i++) {
    mesh_file_t mesh_file;
    
    if (!mesh_file_load(&mesh_file, meshes[i].path))
      return false;
    
    if (!vertex_buffer_new_mesh(
      &renderer->vertex_buffer,
      meshes[i].mesh,
      mesh_file.vertices,
      mesh_file.num_vertices)
    ) {
      return false;
    }
    
    mesh_file_free(&mesh_file);
  }
  
  return true;
}