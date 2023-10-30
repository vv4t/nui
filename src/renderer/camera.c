#include "camera.h"

typedef struct {
  mat4x4_t  mvp;
  mat4x4_t  model;
} ubc_matrices_t;

typedef struct {
  mat4x4_t  mat_project;
  mat4x4_t  mat_view_project;
  
  GLuint    ubo_matrices;
} camera_t;

static camera_t camera;

void camera_init()
{
  glGenBuffers(1, &camera.ubo_matrices);
  glBindBuffer(GL_UNIFORM_BUFFER, camera.ubo_matrices);
  
#ifdef __EMSCRIPTEN__
  glBufferData(GL_UNIFORM_BUFFER, 256, NULL, GL_DYNAMIC_DRAW);
#else
  glBufferData(GL_UNIFORM_BUFFER, sizeof(ubd_matrices_t), NULL, GL_DYNAMIC_DRAW);
#endif
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, camera.ubo_matrices);
  
  camera.mat_projection = mat4x4_init_identity();
  camera.mat_view_projection = mat4x4_init_identity();
}

void view_init_perspective(view_t *view, float aspect_ratio, float fov, float near, float far)
{
  view->aspect_ratio = aspect_ratio;
  view->fov = fov;
  view->near = near;
  view->far = far;
  view->mat_project = mat4x4_init_perspective(aspect_ratio, fov, near, far);
}

void camera_orthogonal(camera_t *camera, float aspect_ratio, float size, float near, float far)
{
  float ar_size = aspect_ratio * size;
  
  camera.mat_projection = mat4x4_init_orthogonal(-size, size, ar_size, -ar_size , near, far);
  camera.mat_view_projection = camera.mat_projection;
}

void camera_move(camera_t *camera, vec3_t view_offset, quat_t view_angle)
{
  vec3_t view_origin = vec3_mulf(view_offset, -1);
  quat_t view_rotation = quat_conjugate(view_angle);
  
  mat4x4_t translation_matrix = mat4x4_init_translation(view_origin);
  mat4x4_t rotation_matrix = mat4x4_init_rotation(view_rotation);
  
  mat4x4_t view_matrix = mat4x4_mul(translation_matrix, rotation_matrix);
  
  camera.mat_view_projection = mat4x4_mul(view_matrix, camera.mat_projection);
}

void camera_sub_data(const camera_t *camera, mat4x4_t mat_model)
{
  mat4x4_t mat_mvp = mat4x4_mul(mat_model, camera.mat_view_projection);
  
  ubd_matrices_t ubd_matrices = {
    .model = mat_model,
    .mvp = mat_mvp
  };
  
  glBindBuffer(GL_UNIFORM_BUFFER, camera.ubo_matrices);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ubd_matrices_t), &ubd_matrices);
}
