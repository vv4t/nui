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

void view_set_offset(view_t *view, vec3_t view_offset, quat_t view_angle);
void view_set_matrix(view_t *view, mat4x4_t view_matrix);

void view_sub_data(const view_t *view, mat4x4_t model_matrix);

#endif
