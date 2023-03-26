#include "game.h"

static void game_look_move(game_t *game, float delta, const usercmd_t *usercmd);

void game_init(game_t *game)
{
  game->position = vec3_init(0.0, 0.0, 0.0);
  game->rotation = quat_init(0.0, 0.0, 0.0, 1.0);
}

void game_update(game_t *game, float delta, const usercmd_t *usercmd)
{
  game->time += delta;
  game_look_move(game, delta, usercmd);
}

void game_look_move(game_t *game, float delta, const usercmd_t *usercmd)
{
  game->camera_yaw += usercmd->d_yaw;
  game->camera_pitch += usercmd->d_pitch;
  
  vec3_t axis_up = vec3_init(0.0f, 1.0f, 0.0f);
  vec3_t axis_right = vec3_init(1.0f, 0.0f, 0.0f);
  
  quat_t rot_yaw = quat_init_rotation(axis_up, game->camera_yaw * GAME_SENSITIVITY);
  
  vec3_t axis_pitch = vec3_rotate(axis_right, rot_yaw);
  quat_t rot_pitch = quat_init_rotation(axis_pitch, game->camera_pitch * GAME_SENSITIVITY);
  
  game->rotation = quat_mul(rot_pitch, rot_yaw);
  
  vec3_t cmd_dir = vec3_init(usercmd->right - usercmd->left, 0.0f, usercmd->forward - usercmd->back);
  vec3_t wish_dir = vec3_rotate(cmd_dir, game->rotation);
  vec3_t move_dir = vec3_mulf(wish_dir, delta);
  
  game->position = vec3_add(game->position, vec3_mulf(move_dir, 4.0f));
}
