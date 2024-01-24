#ifndef RENDERER_API_H
#define RENDERER_API_H

#define SCR_WIDTH 1280
#define SCR_HEIGHT 720

#define VIEW_SCALE 3

#define UBO_CAMERA_BINDING 0
#define UBO_MATERIAL_BINDING 1
#define UBO_LIGHT_BINDING 2

#define TEXTURE_DIFFUSE_BINDING 0
#define TEXTURE_NORMAL_BINDING 1
#define TEXTURE_DEPTH_BINDING 2
#define TEXTURE_SKYBOX_BINDING 3

#define G_BUFFER_POS_BINDING 4
#define G_BUFFER_ALBEDO_BINDING 5
#define G_BUFFER_NORMAL_BINDING 6

#define VIEW_WIDTH (SCR_WIDTH / VIEW_SCALE)
#define VIEW_HEIGHT (SCR_HEIGHT / VIEW_SCALE)

#include "../gl/gl.h"

void renderer_shadow_pass();
void renderer_scene_pass();

#endif
