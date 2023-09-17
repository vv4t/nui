#include "game.h"

#include <limits.h>

static void game_look_move(game_t *game, float delta, const usercmd_t *usercmd);

void game_init(game_t *game)
{
  game->position = vec3_init(0.0, 0.0, 0.0);
  game->rotation = quat_init(0.0, 0.0, 0.0, 1.0);
  
  game->camera_yaw = 0.0;
  game->camera_pitch = 0.0;
  game->time = 0.0;
  
  game->bsp_file = bsp_file_load("assets/bsp/scene.bsp");
}

void game_update(game_t *game, float delta, const usercmd_t *usercmd)
{
  game->time += delta;
  game_look_move(game, delta, usercmd);
}

int bsp_clip_R(bsp_plane_t clips[], bsp_file_t *bsp_file, int node_id, vec3_t pos, bsp_plane_t min_plane)
{
  if (node_id == -1) {
    return 0;
  }
  
  bsp_node_t *node = &bsp_file->nodes[node_id];
  
  vec3_print(node->plane.normal);
  
  float min_dist = vec3_dot(pos, node->plane.normal) - node->plane.distance - 0.5f;
  float max_dist = vec3_dot(pos, node->plane.normal) - node->plane.distance + 0.5f;
  
  int num_clips = 0;
  
  if (max_dist > 0) {
    num_clips += bsp_clip_R(clips, bsp_file, node->ahead, pos, min_plane);
  }
  
  if (min_dist < 0) {
    if (min_dist > min_plane.distance) {
      min_plane.distance = min_dist;
      min_plane.normal = node->plane.normal;
    }
    
    if (node->behind == -1) {
      clips[num_clips++] = min_plane;
    }
    
    num_clips += bsp_clip_R(clips, bsp_file, node->behind, pos, min_plane);
  }
  
  return num_clips;
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
  
  vec3_t next_pos = vec3_add(game->position, vec3_mulf(move_dir, 4.0f));
  
  bsp_plane_t clips[8];
  int num_clips = bsp_clip_R(clips, game->bsp_file, 0, next_pos, (bsp_plane_t) { .distance = INT_MIN });
  
  if (num_clips > 0) {
    printf("%i\n", rand());
  }
  
  game->position = next_pos;
}
