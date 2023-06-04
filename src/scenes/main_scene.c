#include "main_scene.h"

#include "../renderer/mesh_file.h"

static void init_gl();

static bool init_meshes(scene_t *scene);
static bool init_textures(scene_t *scene);
static void init_materials(scene_t *scene);

bool main_scene_init(scene_t *scene, view_t *view)
{
  init_gl();
  
  if (!init_meshes(scene))
    return false;
  
  if (!init_textures(scene))
    return false;
  
  init_materials(scene);
  
  light_t light;
  light.id = 0;
  light.pos = vec3_init(-6.0, 6.0, -6.0);
  light.color = vec4_init(0.0, 1.0, 1.0, 1.0);
  light.intensity = 30.0;
  lights_sub_light(scene->lights, &light, scene, *view);
  
  light.id = 1;
  light.pos = vec3_init(6.0, 6.0, -6.0);
  light.color = vec4_init(1.0, 0.0, 1.0, 1.0);
  light.intensity = 30.0;
  lights_sub_light(scene->lights, &light, scene, *view);
  
  waves_setup(scene->waves);
  
  scene->lights->cube_map = scene->skybox->texture;
  
  return true;
}

void main_scene_render(scene_t *scene, const game_t *game, view_t *view)
{
  waves_move(scene->waves);
  
  hdr_begin(scene->hdr);
  
  glViewport(0, 0, 1280, 720);
  glClear(GL_DEPTH_BUFFER_BIT);
  
  skybox_render(scene->skybox, *view, game->rotation);
  
  lights_bind(scene->lights);
  
  view_set_offset(view, game->position, game->rotation);
  lights_set_view_pos(scene->lights, game->position);
  
  lights_set_material(scene->materials[0]);
  view_sub_data(view, mat4x4_init_identity());
  glDrawArrays(GL_TRIANGLES, scene->meshes[0].offset, scene->meshes[0].count);
  
  lights_set_material(scene->materials[1]);
  view_sub_data(view, mat4x4_init_transform(vec3_init(0.0, -0.2, 0.0), vec3_init(5.0, 1.0, 5.0)));
  glDrawArrays(GL_TRIANGLES, scene->meshes[1].offset, scene->meshes[1].count);
  
  hdr_end(scene->hdr);
  
  hdr_draw(scene->hdr);
}

void main_scene_draw(scene_t *scene, const view_t *view)
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

static bool init_meshes(scene_t *scene)
{
  mesh_file_t mesh_file;
  
  if (!mesh_file_load(&mesh_file, "res/mesh/scene.mesh"))
    return false;
  
  if (
    !buffer_new_mesh(
      scene->buffer,
      &scene->meshes[0],
      mesh_file.vertices,
      mesh_file.num_vertices
    )
  ) {
    return false;
  }
  
  if (!mesh_file_load(&mesh_file, "res/mesh/cube.mesh"))
    return false;
  
  if (
    !buffer_new_mesh(
      scene->buffer,
      &scene->meshes[1],
      mesh_file.vertices,
      mesh_file.num_vertices
    )
  ) {
    return false;
  }
  
  return true;
}

static bool init_textures(scene_t *scene)
{
  if (!texture_load(&scene->textures[0], "res/mtl/tile/color.jpg"))
    return false;
  
  if (!texture_load(&scene->textures[1], "res/mtl/tile/normal.jpg"))
    return false;
  
  if (!texture_load(&scene->textures[2], "res/mtl/water/color.png"))
    return false;
  
  return true;
}

static void init_materials(scene_t *scene)
{
  scene->materials[0].diffuse = scene->textures[0];
  scene->materials[0].normal = scene->textures[1];
  
  scene->materials[1].diffuse = scene->textures[2];
  scene->materials[1].normal = scene->waves->normal_map;
}
