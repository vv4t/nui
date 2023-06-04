#ifndef MAIN_SCENE_H
#define MAIN_SCENE_H

#include "../renderer/scene.h"

bool main_scene_init(scene_t *scene, view_t *view);
void main_scene_render(scene_t *scene, const game_t *game, view_t *view);
void main_scene_draw(scene_t *scene, const view_t *view);

#endif
