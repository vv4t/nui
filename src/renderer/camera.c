#include "camera.h"

typedef struct {
  mat4x4_t  mvp;
  mat4x4_t  model;
} ubc_matrices_t;

typedef struct {
  view_t    view;
  
  mat4x4_t  mat_project;
  mat4x4_t  mat_view_project;
  
  GLuint    ubo_matrices;
} camera_t;

static camera_t camera;

void camera_init()
{
  camera.view = (view_t) { .width = 1.0, .height = 1.0, .mat_project = mat4x4_init_identity() };
  
  glGenBuffers(1, &camera.ubo_matrices);
  glBindBuffer(GL_UNIFORM_BUFFER, camera.ubo_matrices);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(ubc_matrices_t), NULL, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, camera.ubo_matrices);
}

void camera_set_view(view_t view)
{
  camera.view = view;
}

void camera_move(vec3_t view_offset, quat_t view_angle)
{
  vec3_t view_origin = vec3_mulf(view_offset, -1);
  quat_t view_rotation = quat_conjugate(view_angle);
  
  mat4x4_t translation_matrix = mat4x4_init_translation(view_origin);
  mat4x4_t rotation_matrix = mat4x4_init_rotation(view_rotation);
  
  mat4x4_t view_matrix = mat4x4_mul(translation_matrix, rotation_matrix);
  
  camera.mat_view_project = mat4x4_mul(view_matrix, camera.view.mat_project);
}

void camera_model(mat4x4_t mat_model)
{
  mat4x4_t mat_mvp = mat4x4_mul(mat_model, camera.mat_view_project);
  
  ubc_matrices_t ubc_matrices = { .model = mat_model, .mvp = mat_mvp };
  
  glBindBuffer(GL_UNIFORM_BUFFER, camera.ubo_matrices);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ubc_matrices), &ubc_matrices);
}

void view_init_perspective(view_t *view, float fov, float width, float height, float near, float far)
{
  float aspect_ratio = (float) width / (float) height;
  
  view->width = width;
  view->height = height;
  view->mat_project = mat4x4_init_perspective(aspect_ratio, fov, near, far);
}

void view_init_orthogonal(view_t *view, float width, float height, float near, float far)
{
  float aspect_ratio = (float) width / (float) height;
  
  view->width = width;
  view->height = height;
  view->mat_project = mat4x4_init_orthogonal(-1.0, 1.0, aspect_ratio, -aspect_ratio, near, far);
}
