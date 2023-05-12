#include "scene.h"

bool material_load(material_t *material, const char *src_diffuse, const char *src_normal)
{
  if (!texture_load(&material->diffuse, src_diffuse))
    return false;
  
  if (!texture_load(&material->normal, src_normal))
    return false;
  
  return true;
}

void draw_scene(scene_t *scene)
{
  scene->draw(scene->data, scene->view);
}

void draw_mesh(mesh_t mesh)
{
  glDrawArrays(GL_TRIANGLES, mesh.ptr, mesh.num_vertices);
}
