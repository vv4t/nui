#include "camera.h"

#include "api.h"

typedef struct {
  mat4x4_t mat_mvp;
  mat4x4_t mat_model;
  mat4x4_t mat_view;
  mat4x4_t mat_look;
  vec3_t view_pos;
} ub_camera_t;

typedef struct {
  view_t view;
  GLuint ubo_camera;
  mat4x4_t mat_view;
  mat4x4_t mat_look;
  mat4x4_t mat_projection;
  vec3_t view_pos;
} camera_t;

static camera_t camera;

void camera_init()
{
  glGenBuffers(1, &camera.ubo_camera);
  glBindBuffer(GL_UNIFORM_BUFFER, camera.ubo_camera);
  glBufferData(GL_UNIFORM_BUFFER, 512, NULL, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, UBO_CAMERA_BINDING, camera.ubo_camera);
}

void camera_set_view(view_t view)
{
  camera.view = view;
}

void camera_set_viewport(int x, int y, int width, int height)
{
  glViewport(x, y, width, height);
  glScissor(x, y, width, height);
}

void camera_move(vec3_t view_offset, quat_t view_angle)
{
  vec3_t view_origin = vec3_mulf(view_offset, -1);
  quat_t view_rotation = quat_conjugate(view_angle);
  
  mat4x4_t translation_matrix = mat4x4_init_translation(view_origin);
  mat4x4_t rotation_matrix = mat4x4_init_rotation(view_rotation);
  
  mat4x4_t view_matrix = mat4x4_mul(translation_matrix, rotation_matrix);
  
  camera.mat_view = view_matrix;
  camera.mat_look = mat4x4_mul(rotation_matrix, camera.view.mat_project);
  camera.mat_projection = mat4x4_mul(view_matrix, camera.view.mat_project);
  camera.view_pos = view_offset;
}

void camera_look_at(vec3_t at, vec3_t from, vec3_t up)
{
  mat4x4_t view_matrix = mat4x4_init_look_at(at, from, up);
  mat4x4_t look_matrix = mat4x4_init_look_at(vec3_sub(at, from), vec3_init(0.0, 0.0, 0.0), up);
  
  camera.mat_view = view_matrix;
  camera.mat_look = mat4x4_mul(look_matrix, camera.view.mat_project);
  camera.mat_projection = mat4x4_mul(view_matrix, camera.view.mat_project);
  camera.view_pos = from;
}

void camera_model(mat4x4_t mat_model)
{
  mat4x4_t mat_mvp = mat4x4_mul(mat_model, camera.mat_projection);
  
  ub_camera_t ub_camera = {
    .mat_model = mat_model,
    .mat_mvp = mat_mvp,
    .mat_view = camera.mat_view,
    .mat_look = camera.mat_look,
    .view_pos = camera.view_pos
  };
  
  glBindBuffer(GL_UNIFORM_BUFFER, camera.ubo_camera);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ub_camera_t), &ub_camera);
}

mat4x4_t camera_get_view_project()
{
  return camera.mat_projection;
}

void camera_shader_setup(GLuint shader)
{
  glUseProgram(shader);
  
  GLuint ubl_camera = glGetUniformBlockIndex(shader, "ub_camera");
  glUniformBlockBinding(shader, ubl_camera, UBO_CAMERA_BINDING);
}

void view_set_perspective(view_t *view, float aspect_ratio, float fov, float near, float far)
{
  view->mat_project = mat4x4_init_perspective(aspect_ratio, fov, near, far);
}
