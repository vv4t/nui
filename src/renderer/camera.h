#ifndef CAMERA_H
#define CAMERA_H

#include "../gl/gl.h"
#include "../common/nui_math.h"

typedef struct {
  mat4x4_t mat_project;
} view_t;

void view_set_perspective(view_t *view, float aspect_ratio, float fov, float near, float far);

void camera_init();
void camera_set_viewport(int x, int y, int width, int height);
void camera_set_view(view_t view);
void camera_move(vec3_t view_offset, quat_t view_angle);
void camera_look_at(vec3_t at, vec3_t from, vec3_t up);
void camera_model(mat4x4_t model_matrix);
mat4x4_t camera_get_view_project();
void camera_shader_setup(GLuint shader);

#endif
