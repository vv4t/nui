#ifndef SKYBOX_H
#define SKYBOX_H

#include "gl.h"
#include "nui_math.h"
#include "mesh.h"
#include <stdbool.h>

typedef struct {
  GLuint    texture;
  GLuint    shader;
  GLuint    ul_mvp;
  mesh_t    mesh;
} skybox_t;

bool skybox_init(skybox_t *skybox, vertex_buffer_t *vertex_buffer);
void skybox_render(skybox_t *skybox, mat4x4_t projection_matrix, quat_t view_angle);

#endif
