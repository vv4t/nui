#include "game.h"

#include <limits.h>

void game_init(game_t *game)
{
  player_init(&game->player);
}

void game_map_load(game_t *game, map_t *map)
{
  game->bsp = map_load_bsp(map);
} 

void game_update(game_t *game, const usercmd_t *usercmd)
{
  player_look(&game->player, usercmd);
  player_move(&game->player, game->bsp, usercmd);
}
