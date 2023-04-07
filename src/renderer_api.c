#include "renderer_api.h"

bool material_load(material_t *material, const char *src_color, const char *src_normal)
{
  if (!texture_load(&material->color, src_color))
    return false;
  
  if (!texture_load(&material->normal, src_normal))
    return false;
  
  return true;
}

void do_draw_call(draw_call_t *draw_call, mat4x4_t view_projection_matrix, vec3_t view_pos)
{
  draw_call->draw(draw_call->data, draw_call->ubo_matrices, view_projection_matrix, view_pos);
}

void draw_mesh(mesh_t mesh)
{
  glDrawArrays(GL_TRIANGLES, mesh.ptr, mesh.num_vertices);
}

void set_matrices(
  GLuint    ubo_matrices,
  mat4x4_t  model_matrix,
  mat4x4_t  view_projection_matrix,
  vec3_t    view_pos)
{
  mat4x4_t mvp_matrix = mat4x4_mul(model_matrix, view_projection_matrix);
  
  ub_matrices_t ub_matrices = {
    .model = model_matrix,
    .mvp = mvp_matrix,
    .view_pos = view_pos
  };
  
  glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ub_matrices_t), &ub_matrices);
}
