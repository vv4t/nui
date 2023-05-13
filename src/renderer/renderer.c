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

static void renderer_render_scene(renderer_t *renderer, const game_t *game);

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
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
  
  vertex_buffer_init(&renderer->vertex_buffer, 4096);
  
  view_init(&renderer->view);
  view_perspective(&renderer->view, 720.0 / 1280.0, to_radians(90.0), 0.1, 100.0);
}

static bool renderer_init_shaders(renderer_t *renderer)
{
  if (!lights_init(&renderer->lights))
    return false;
  
  if (!colors_init(&renderer->colors))
    return false;
  
  if (!hdr_init(&renderer->hdr, &renderer->vertex_buffer))
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
  
  lights_set_scene(&renderer->lights, &renderer->scene);
  
  lights_new_light(&renderer->lights, &renderer->light1);
    renderer->light1.pos = vec3_init(0.0, 4.0, 0.0);
    renderer->light1.color = vec4_init(0.0, 1.0, 0.5, 0.2);
    renderer->light1.intensity = 10.0;
  lights_sub_light(&renderer->lights, &renderer->light1);
  
  lights_new_light(&renderer->lights, &renderer->light2);
    renderer->light2.pos = vec3_init(3.0, 1.3, 3.0);
    renderer->light2.color = vec4_init(1.0, 0.0, 0.5, 0.2);
    renderer->light2.intensity = 10.0;
  lights_sub_light(&renderer->lights, &renderer->light2);
  
  return true;
}

void renderer_render(renderer_t *renderer, const game_t *game)
{
  hdr_begin(&renderer->hdr);
    renderer_render_scene(renderer, game);
  hdr_end(&renderer->hdr);
  
  hdr_draw(&renderer->hdr);
}

static void renderer_render_scene(renderer_t *renderer, const game_t *game)
{
  glViewport(0, 0, 1280, 720);
  glClear(GL_DEPTH_BUFFER_BIT);
  
  skybox_render(&renderer->skybox, &renderer->view, game->rotation);
  
  view_move(&renderer->view, game->position, game->rotation);
  
  lights_bind(&renderer->lights);
    lights_set_material(&renderer->tile_mtl);
    view_sub_data(&renderer->view, mat4x4_init_identity());
    draw_mesh(renderer->scene_mesh);
  
  colors_bind(&renderer->colors);
    mat4x4_t light1_matrix = mat4x4_mul(
      mat4x4_init_scale(vec3_init(0.5, 0.5, 0.5)),
      mat4x4_init_translation(renderer->light1.pos)
    );
    
    colors_set_color(&renderer->colors, renderer->light1.color);
    view_sub_data(&renderer->view, light1_matrix);
    draw_mesh(renderer->cube_mesh);
    
    mat4x4_t light2_matrix = mat4x4_mul(
      mat4x4_init_scale(vec3_init(0.5, 0.5, 0.5)),
      mat4x4_init_translation(renderer->light2.pos)
    );
    
    colors_set_color(&renderer->colors, renderer->light2.color);
    view_sub_data(&renderer->view, light2_matrix);
    draw_mesh(renderer->cube_mesh);
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
      &renderer->stone_mtl,
      "res/mtl/stone/color.jpg",
      "res/mtl/stone/normal.jpg"
    )
  ) {
    return false;
  }
  
  if (
    !material_load(
      &renderer->tile_mtl,
      "res/mtl/tile/color.jpg",
      "res/mtl/tile/normal.jpg"
    )
  ) {
    return false;
  }
  
  return true;
}

static bool renderer_init_mesh(renderer_t *renderer)
{
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
