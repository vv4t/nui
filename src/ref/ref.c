#include "ref.h"

#include "gl.h"
#include "mesh_file.h"
#include "../common/file.h"

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
  view_perspective(&ref->view, 720.0 / 1280.0, to_radians(90.0), 0.1, 100.0);
  
  ref_init_gl(ref);
  
  if (!ref_init_shaders(ref))
    return false;
  
  if (!ref_init_scene(ref))
    return false;
  
  return true;
}

static void ref_init_gl(ref_t *ref)
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
}

static bool ref_init_shaders(ref_t *ref)
{
  if (!lights_init(&ref->lights))
    return false;
  
  if (!skybox_init(&ref->skybox, &ref->buffer))
    return false;
  
  return true;
}

static bool ref_init_scene(ref_t *ref)
{
  if (!ref_init_mesh(ref))
    return false;
  
  if (!ref_init_texture(ref))
    return false;
  
  if (!ref_init_material(ref))
    return false;
  
  ref->lights.shadow_pass.data = ref;
  ref->lights.shadow_pass.draw = ref_light_pass;
  
  light_t light;
  lights_new_light(&ref->lights, &light);
  light.pos = vec3_init(0.0, 5.0, 0.0);
  light.intensity = 40.0;
  lights_sub_light(&ref->lights, &light);
  
  return true;
}

void ref_render(ref_t *ref, const game_t *game)
{
  glViewport(0, 0, 1280, 720);
  glClear(GL_DEPTH_BUFFER_BIT);
  
  skybox_render(&ref->skybox, &ref->view, game->rotation);
  
  lights_bind(&ref->lights);
  ref_render_scene(ref, game);
}

static void ref_render_scene(ref_t *ref, const game_t *game)
{
  view_move(&ref->view, game->position, game->rotation);
  lights_set_view_pos(&ref->lights, game->position);
  lights_set_material(&ref->tile_mtl);
  view_sub_data(&ref->view, mat4x4_init_identity());
  glDrawArrays(GL_TRIANGLES, ref->scene_mesh.offset, ref->scene_mesh.count);
}

static void ref_light_pass(void *data, mat4x4_t light_matrix)
{
  ref_t *ref = (ref_t*) data;
  
  view_set(&ref->view, light_matrix);
  view_sub_data(&ref->view, mat4x4_init_identity());
  glDrawArrays(GL_TRIANGLES, ref->scene_mesh.offset, ref->scene_mesh.count);
}

static bool ref_init_texture(ref_t *ref)
{
  if (!texture_load(&ref->tile_diffuse_tex, "res/mtl/tile/color.jpg"))
    return false;
  
  if (!texture_load(&ref->tile_normal_tex, "res/mtl/tile/normal.jpg"))
    return false;
  
  return true;
}

static bool ref_init_material(ref_t *ref)
{
  ref->tile_mtl.diffuse = ref->tile_diffuse_tex;
  ref->tile_mtl.normal = ref->tile_normal_tex;
  
  return true;
}

static bool ref_init_mesh(ref_t *ref)
{
  struct {
    const char  *path;
    mesh_t      *mesh;
  } meshes[] = {
    { .path = "res/mesh/scene.mesh", &ref->scene_mesh },
    { .path = "res/mesh/cube.mesh", &ref->cube_mesh },
  };
  
  for (int i = 0; i < 2; i++) {
    mesh_file_t mesh_file;
    
    if (!mesh_file_load(&mesh_file, meshes[i].path))
      return false;
    
    if (
      !buffer_new_mesh(
        &ref->buffer,
        meshes[i].mesh,
        mesh_file.vertices,
        mesh_file.num_vertices
      )
    ) {
      return false;
    }
    
    mesh_file_free(&mesh_file);
  }
  
  return true;
}
