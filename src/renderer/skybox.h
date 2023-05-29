#ifndef SKYBOX_H
#define SKYBOX_H

#include "gl.h"
#include "view.h"
#include "renderer_def.h"
#include "buffer.h"
#include "../common/nui_math.h"
#include <stdbool.h>

typedef struct {
  texture_t texture;
  shader_t  shader;
  mesh_t    mesh;
} skybox_t;

bool skybox_init(skybox_t *skybox, buffer_t *buffer);
void skybox_render(skybox_t *skybox, view_t view, quat_t view_angle);

#endif
