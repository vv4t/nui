#ifndef RENDERER_API_H
#define RENDERER_API_H

#include "camera.h"

typedef struct renderer_s renderer_t;

void renderer_light_pass(renderer_t *renderer, camera_t camera);
void renderer_scene_pass(renderer_t *renderer, camera_t camera);

#endif
