#ifndef MAIN_PIPELINE_H
#define MAIN_PIPELINE_H

#include "../renderer/pipeline.h"

bool main_pipeline_init_scene(pipeline_t *pipeline, scene_t *scene, view_t *view);
void main_pipeline_render_scene(pipeline_t *pipeline, const scene_t *scene, const game_t *game, view_t *view);

#endif
