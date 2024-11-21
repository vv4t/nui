#ifndef GAME_H
#define GAME_H

#include "../common/nui_math.h"
#include "usercmd.h"
#include "map.h"
#include "bsp.h"
#include "player.h"

typedef struct {
  bsp_t bsp;
  player_t player;
  
  vec3_t light_pos;
  bool light_update;
} game_t;

void game_init(game_t *game);
void game_map_load(game_t *game, map_t *map);
void game_update(game_t *game, const usercmd_t *usercmd);
void game_exit();

#endif
