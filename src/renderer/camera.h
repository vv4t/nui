#ifndef CAMERA_H
#define CAMERA_H

#include "gl.h"
#include "../common/nui_math.h"

typedef struct {
  float aspect_ratio;
  float near;
  float far;
  mat4x4_t mat_project;
} view_t;

void view_init_perspective(view_t *view, float fov, float width, flaot height, float near, float far);

void camera_init();
void camera_aspect_ratio(float aspect_ratio);
void camera_perspective(float fov, float near, float far);
void camera_orthogonal(float size, float near, float far);
void camera_move(vec3_t view_offset, quat_t view_angle);
void camera_sub_data(mat4x4_t model_matrix);

#endif
