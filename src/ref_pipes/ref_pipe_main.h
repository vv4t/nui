#ifndef REF_PIPE_MAIN_H
#define REF_PIPE_MAIN_H

#include "../ref/ref_pipe.h"

bool ref_pipe_main_init_scene(ref_pipe_t *ref_pipe, scene_t *scene, view_t *view);
void ref_pipe_main_render_scene(ref_pipe_t *ref_pipe, const scene_t *scene, const game_t *game, view_t *view);

#endif
