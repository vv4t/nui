#include "game.h"

#include <limits.h>

void game_init(game_t *game)
{
  player_init(&game->player);
}

void game_map_load(game_t *game, map_t *map)
{
  bsp_load(&game->bsp, map);
} 

void game_update(game_t *game, const usercmd_t *usercmd)
{
  player_look(&game->player, usercmd);
  player_move(&game->player, &game->bsp, usercmd);
  
  if (usercmd->attack1) {
    vec3_t d = vec3_rotate(vec3_init(0.0, 0.0, 4.0), game->player.rotation);
    game->light_pos = vec3_add(game->player.position, d);
    game->light_update = true;
  } else {
    game->light_update = false;
  }
}
