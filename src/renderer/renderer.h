#ifndef RENDERER_H
#define RENDERER_H

#include "../game/game.h"
#include "../game/map.h"

bool renderer_init(const game_t *game);
void renderer_map_load(const map_t *map);
void renderer_render();

#endif
