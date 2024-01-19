#ifndef RENDERER_API_H
#define RENDERER_API_H

#define SCR_WIDTH 1280
#define SCR_HEIGHT 720

#define VIEW_SCALE 2

#define VIEW_WIDTH (SCR_WIDTH / VIEW_SCALE)
#define VIEW_HEIGHT (SCR_HEIGHT / VIEW_SCALE)

#include "../gl/gl.h"

void renderer_shadow_pass();
void renderer_scene_pass();

#endif
