#include "main_pipeline.h"

#include "../ref/mesh_file.h"

static void init_gl();

static bool init_meshes(pipeline_t *pipeline, scene_t *scene);
static bool init_textures(pipeline_t *pipeline, scene_t *scene);
static void init_materials(pipeline_t *pipeline, scene_t *scene);

static void draw_scene(const scene_t *scene, const view_t *view);

bool main_pipeline_init_scene(pipeline_t *pipeline, scene_t *scene, view_t *view)
{
  init_gl();
  
  if (!init_meshes(pipeline, scene))
    return false;
  
  if (!init_textures(pipeline, scene))
    return false;
  
  init_materials(pipeline, scene);
  
  scene->draw = draw_scene;
  
  light_t light;
  lights_new_light(pipeline->lights, &light);
  light.pos = vec3_init(0.0, 5.0, 0.0);
  light.intensity = 40.0;
  lights_sub_light(pipeline->lights, &light, scene, *view);
  
  return true;
}

void main_pipeline_render_scene(pipeline_t *pipeline, const scene_t *scene, const game_t *game, view_t *view)
{
  hdr_begin(pipeline->hdr);
  glViewport(0, 0, 1280, 720);
  glClear(GL_DEPTH_BUFFER_BIT);
  
  skybox_render(pipeline->skybox, *view, game->rotation);
  
  lights_bind(pipeline->lights);
  
  view_set_offset(view, game->position, game->rotation);
  lights_set_view_pos(pipeline->lights, game->position);
  lights_set_material(scene->materials[0]);
  
  view_sub_data(view, mat4x4_init_identity());
  glDrawArrays(GL_TRIANGLES, scene->meshes[0].offset, scene->meshes[0].count);
  hdr_end(pipeline->hdr);
  
  hdr_draw(pipeline->hdr);
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

static bool init_meshes(pipeline_t *pipeline, scene_t *scene)
{
  mesh_file_t mesh_file;
  
  if (!mesh_file_load(&mesh_file, "res/mesh/scene.mesh"))
    return false;
  
  if (
    !buffer_new_mesh(
      pipeline->buffer,
      &scene->meshes[0],
      mesh_file.vertices,
      mesh_file.num_vertices
    )
  ) {
    return false;
  }
  
  return true;
}

static bool init_textures(pipeline_t *pipeline, scene_t *scene)
{
  if (!texture_load(&scene->textures[0], "res/mtl/tile/color.jpg"))
    return false;
  
  if (!texture_load(&scene->textures[1], "res/mtl/tile/normal.jpg"))
    return false;
  
  return true;
}

static void init_materials(pipeline_t *pipeline, scene_t *scene)
{
  scene->materials[0].diffuse = scene->textures[0];
  scene->materials[0].normal = scene->textures[1];
}
