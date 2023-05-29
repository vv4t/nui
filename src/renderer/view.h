#ifndef VIEW_H
#define VIEW_H

#include "gl.h"
#include "../common/nui_math.h"

typedef struct {
  mat4x4_t  mvp;
  mat4x4_t  model;
} ubc_view_t;

typedef struct {
  mat4x4_t  projection_matrix;
  mat4x4_t  view_projection_matrix;
  GLuint    ubo_view;
} view_t;

void view_init(view_t *view);

void view_perspective(view_t *view, float aspect_ratio, float fov, float near, float far);
void view_move(view_t *view, vec3_t view_offset, quat_t view_angle);
void view_set(view_t *view, mat4x4_t view_projection_matrix);

void view_sub_data(view_t *view, mat4x4_t model_matrix);

#endif
