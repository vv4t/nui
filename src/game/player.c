#include "player.h"

#define LOOK_SENSITIVITY 0.01

void player_init(player_t *p)
{
  p->position = vec3_init(0.0, 1.0, 0.0);
  p->rotation = quat_init(0.0, 0.0, 0.0, 1.0);
  
  p->yaw = 0.0;
  p->pitch = 0.0;
}

void player_move(player_t *p, const bsp_t *bsp, const usercmd_t *usercmd)
{
  vec3_t cmd_dir = vec3_init(usercmd->right - usercmd->left, 0.0f, usercmd->forward - usercmd->back);
  vec3_t wish_dir = vec3_rotate(cmd_dir, p->rotation);
  vec3_t move_dir = vec3_mulf(wish_dir, 0.015);
  
  vec3_t delta_pos = vec3_mulf(move_dir, 4.0f);
  vec3_t next_pos = vec3_add(p->position, delta_pos);
  
  sphere_t sphere = { .pos = p->position, .radius = 0.5f };
  
  clip_t clips[8];
  int num_clips = bsp_clip_sphere(clips, bsp, &sphere);
  
  for (int i = 0; i < num_clips; i++) {
    float lambda = -vec3_dot(delta_pos, clips[i].normal);
    if (lambda > 0) {
      vec3_t slide = vec3_mulf(clips[i].normal, lambda);
      delta_pos = vec3_add(delta_pos, slide);
    }
  }
  
  p->position = vec3_add(p->position, delta_pos);
}

void player_look(player_t *p, const usercmd_t *usercmd)
{
  p->yaw += usercmd->d_yaw;
  p->pitch += usercmd->d_pitch;
  
  vec3_t axis_up = vec3_init(0.0f, 1.0f, 0.0f);
  vec3_t axis_right = vec3_init(1.0f, 0.0f, 0.0f);
  
  quat_t rot_yaw = quat_init_rotation(axis_up, p->yaw * LOOK_SENSITIVITY);
  
  vec3_t axis_pitch = vec3_rotate(axis_right, rot_yaw);
  quat_t rot_pitch = quat_init_rotation(axis_pitch, p->pitch * LOOK_SENSITIVITY);
  
  p->rotation = quat_mul(rot_pitch, rot_yaw);
}
