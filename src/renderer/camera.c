#include "camera.h"

typedef struct {
  mat4x4_t  mvp;
  mat4x4_t  model;
} ub_matrices_t;

typedef struct {
  view_t    view;
  
  mat4x4_t  mat_project;
  mat4x4_t  mat_view_project;
  
  GLuint    ubo_matrices;
} camera_t;

static camera_t camera;

void camera_init()
{
  view_set_viewport(&camera.view, 0, 0, 800, 600);
  view_set_orthogonal(&camera.view, -1.0, 1.0);
  
  glGenBuffers(1, &camera.ubo_matrices);
  glBindBuffer(GL_UNIFORM_BUFFER, camera.ubo_matrices);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(ub_matrices_t), NULL, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, camera.ubo_matrices);
}

void camera_set_view(view_t view)
{
  camera.view = view;
  glViewport(view.left, view.top, view.left + view.width, view.top + view.height);
  glScissor(view.left, view.top, view.left + view.width, view.top + view.height);
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

void camera_look_at(vec3_t at, vec3_t from, vec3_t up)
{
  mat4x4_t view_matrix = mat4x4_init_look_at(at, from, up);
  camera.mat_view_project = mat4x4_mul(view_matrix, camera.view.mat_project);
}

void camera_model(mat4x4_t mat_model)
{
  mat4x4_t mat_mvp = mat4x4_mul(mat_model, camera.mat_view_project);
  
  ub_matrices_t ub_matrices = { .model = mat_model, .mvp = mat_mvp };
  
  glBindBuffer(GL_UNIFORM_BUFFER, camera.ubo_matrices);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ub_matrices), &ub_matrices);
}

mat4x4_t camera_get_mat_vp()
{
  return camera.mat_view_project;
}

void view_set_viewport(view_t *view, float left, float top, float width, float height)
{
  view->left = left;
  view->top = top;
  view->width = width;
  view->height = height;
}

void view_set_perspective(view_t *view, float fov, float near, float far)
{
  float aspect_ratio = (float) view->height / (float) view->width; 
  view->mat_project = mat4x4_init_perspective(aspect_ratio, fov, near, far);
}

void view_set_orthogonal(view_t *view, float near, float far)
{
  float aspect_ratio = (float) view->width / (float) view->height;
  view->mat_project = mat4x4_init_orthogonal(-1.0, 1.0, aspect_ratio, -aspect_ratio, near, far);
}
