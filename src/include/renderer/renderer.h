#ifndef RENDERER_H
#define RENDERER_H

#include <game/game.h>

void renderer_init();
void renderer_render(const game_t *g);
void renderer_deinit();

#endif
