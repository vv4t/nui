#include "view.h"

void view_init(view_t *view)
{
  glGenBuffers(1, &view->ubo_view);
  glBindBuffer(GL_UNIFORM_BUFFER, view->ubo_view);
#ifdef __EMSCRIPTEN__
  glBufferData(GL_UNIFORM_BUFFER, 256, NULL, GL_DYNAMIC_DRAW);
#else
  glBufferData(GL_UNIFORM_BUFFER, sizeof(ubc_view_t), NULL, GL_DYNAMIC_DRAW);
#endif
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, view->ubo_view); 
}

void view_perspective(view_t *view, float aspect_ratio, float fov, float near, float far)
{
  view->projection_matrix = mat4x4_init_perspective(
    aspect_ratio,
    fov,
    near,
    far
  );
}

void view_move(view_t *view, vec3_t view_offset, quat_t view_angle)
{
  vec3_t view_origin = vec3_mulf(view_offset, -1);
  quat_t view_rotation = quat_conjugate(view_angle);
  
  mat4x4_t translation_matrix = mat4x4_init_translation(view_origin);
  mat4x4_t rotation_matrix = mat4x4_init_rotation(view_rotation);
  
  mat4x4_t view_matrix = mat4x4_mul(translation_matrix, rotation_matrix);
  
  view->view_projection_matrix = mat4x4_mul(view_matrix, view->projection_matrix);
}

void view_set(view_t *view, mat4x4_t view_projection_matrix)
{
  view->view_projection_matrix = view_projection_matrix;
}

void view_sub_data(view_t *view, mat4x4_t model_matrix)
{
  mat4x4_t mvp_matrix = mat4x4_mul(model_matrix, view->view_projection_matrix);
  
  ubc_view_t ubc_view = {
    .model = model_matrix,
    .mvp = mvp_matrix
  };
  
  glBindBuffer(GL_UNIFORM_BUFFER, view->ubo_view);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ubc_view_t), &ubc_view);
}
