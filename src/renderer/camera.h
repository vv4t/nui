#ifndef CAMERA_H
#define CAMERA_H

#include "gl.h"
#include "../common/nui_math.h"

typedef struct {
  float width;
  float height;
  mat4x4_t mat_project;
} view_t;

void view_init_perspective(view_t *view, float fov, float width, float height, float near, float far);
void view_init_orthogonal(view_t *view, float width, float height, float near, float far);

void camera_init();
void camera_set_view(view_t view);
void camera_move(vec3_t view_offset, quat_t view_angle);
void camera_look_at(vec3_t at, vec3_t from, vec3_t up);
void camera_model(mat4x4_t model_matrix);

#endif
