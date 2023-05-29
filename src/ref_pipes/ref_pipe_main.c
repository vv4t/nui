#include "ref_pipe_main.h"

#include "../ref/mesh_file.h"

static void init_gl();

static bool init_meshes(ref_pipe_t *ref_pipe, scene_t *scene);
static bool init_textures(ref_pipe_t *ref_pipe, scene_t *scene);
static void init_materials(ref_pipe_t *ref_pipe, scene_t *scene);

static void draw_scene(const scene_t *scene, const view_t *view);

bool ref_pipe_main_init_scene(ref_pipe_t *ref_pipe, scene_t *scene, view_t *view)
{
  init_gl();
  
  if (!init_meshes(ref_pipe, scene))
    return false;
  
  if (!init_textures(ref_pipe, scene))
    return false;
  
  init_materials(ref_pipe, scene);
  
  scene->draw = draw_scene;
  
  light_t light;
  lights_new_light(ref_pipe->lights, &light);
  light.pos = vec3_init(0.0, 5.0, 0.0);
  light.intensity = 40.0;
  lights_sub_light(ref_pipe->lights, &light, scene, *view);
  
  return true;
}

void ref_pipe_main_render_scene(ref_pipe_t *ref_pipe, const scene_t *scene, const game_t *game, view_t *view)
{
  glViewport(0, 0, 1280, 720);
  glClear(GL_DEPTH_BUFFER_BIT);
  
  skybox_render(ref_pipe->skybox, *view, game->rotation);
  
  lights_bind(ref_pipe->lights);
  
  view_set_offset(view, game->position, game->rotation);
  lights_set_view_pos(ref_pipe->lights, game->position);
  lights_set_material(scene->materials[0]);
  
  view_sub_data(view, mat4x4_init_identity());
  glDrawArrays(GL_TRIANGLES, scene->meshes[0].offset, scene->meshes[0].count);
}

static void draw_scene(const scene_t *scene, const view_t *view)
{
  view_sub_data(view, mat4x4_init_identity());
  glDrawArrays(GL_TRIANGLES, scene->meshes[0].offset, scene->meshes[0].count);
}

static void init_gl()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
}

static bool init_meshes(ref_pipe_t *ref_pipe, scene_t *scene)
{
  mesh_file_t mesh_file;
  
  if (!mesh_file_load(&mesh_file, "res/mesh/scene.mesh"))
    return false;
  
  if (
    !buffer_new_mesh(
      ref_pipe->buffer,
      &scene->meshes[0],
      mesh_file.vertices,
      mesh_file.num_vertices
    )
  ) {
    return false;
  }
  
  return true;
}

static bool init_textures(ref_pipe_t *ref_pipe, scene_t *scene)
{
  if (!texture_load(&scene->textures[0], "res/mtl/tile/color.jpg"))
    return false;
  
  if (!texture_load(&scene->textures[1], "res/mtl/tile/normal.jpg"))
    return false;
  
  return true;
}

static void init_materials(ref_pipe_t *ref_pipe, scene_t *scene)
{
  scene->materials[0].diffuse = scene->textures[0];
  scene->materials[0].normal = scene->textures[1];
}
