#ifndef CAMERA_H
#define CAMERA_H

#include "gl.h"
#include "../common/nui_math.h"

typedef struct {
  mat4x4_t  mat_projection;
  mat4x4_t  mat_view_projection;
  
  GLuint    ubo_matrices;
} camera_t;

void camera_init(camera_t *camera);
void camera_perspective(camera_t *camera, float fov, float aspect_ratio, float near, float far);
void camera_orthogonal(camera_t *camera, float size, float aspect_ratio, float near, float far);
void camera_move(camera_t *camera, vec3_t view_offset, quat_t view_angle);
void camera_sub_data(const camera_t *camera, mat4x4_t model_matrix);

#endif
