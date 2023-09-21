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
  player_move(&game->player, usercmd);
}

void game_look_move(game_t *game, float delta, const usercmd_t *usercmd)
{
  /*
  sphere_t sphere = { .pos = game->position, .radius = 0.5f };
  
  clip_t clips[8];
  int num_clips = bsp_clip_sphere(clips, game->bsp, &sphere);
  
  for (int i = 0; i < num_clips; i++) {
    float lambda = -vec3_dot(delta_pos, clips[i].normal);
    if (lambda > 0) {
      vec3_t slide = vec3_mulf(clips[i].normal, lambda);
      delta_pos = vec3_add(delta_pos, slide);
    }
  }*/
}
