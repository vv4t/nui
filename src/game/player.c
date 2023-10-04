#include "player.h"

#define LOOK_SENSITIVITY 0.01

void player_accelerate(player_t *p, vec3_t wish_dir, float accel, float wish_speed);
void player_air_accelerate(player_t *p, vec3_t wish_dir, float wish_speed);

static hull_t player_hull = {
  .pos = { .x = 0.0, .y = 0.0, .z = 0.0 },
  .vertices = {
    { .x = -0.2, .y = 0.3, .z = -0.2 },
    { .x = +0.2, .y = 0.3, .z = -0.2 },
    { .x = -0.2, .y = 0.3, .z = +0.2 },
    { .x = +0.2, .y = 0.3, .z = +0.2 },
    
    { .x = -0.2, .y = -0.7, .z = -0.2 },
    { .x = +0.2, .y = -0.7, .z = -0.2 },
    { .x = -0.2, .y = -0.7, .z = +0.2 },
    { .x = +0.2, .y = -0.7, .z = +0.2 }
  },
  .num_vertices = 8
};

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
  wish_dir.y = 0.0f;
  
  if (p->ground) {
    player_accelerate(p, wish_dir, 4.0, 6.0);
    p->velocity = vec3_mulf(p->velocity, 0.9f);
    
    if (usercmd->jump) {
      p->velocity.y += 3.0f;
    }
  } else {
    p->velocity.y -= 9.8 * 0.015;
    player_air_accelerate(p, wish_dir, 0.6);
  }
  
  p->position = vec3_add(p->position, vec3_mulf(p->velocity, 0.015));
  player_hull.pos = p->position;
  
  clip_t clips[32];
  int num_clips = bsp_clip_hull(clips, bsp, &player_hull);
  
  p->ground = false;
  for (int i = 0; i < num_clips; i++) {
    float lambda_pos = -(hull_furthest_in(&player_hull, clips[i].normal) - clips[i].distance);
    float lambda_vel = -vec3_dot(p->velocity, clips[i].normal);
    
    if (lambda_vel > 0) {
      if (clips[i].normal.y > 0.4f) {
        p->ground = true;
      }
      
      vec3_t slide_pos = vec3_mulf(clips[i].normal, lambda_pos);
      vec3_t slide_vel = vec3_mulf(clips[i].normal, lambda_vel);
      
      p->position = vec3_add(p->position, slide_pos);
      p->velocity = vec3_add(p->velocity, slide_vel);
    }
  }
}

void player_accelerate(player_t *p, vec3_t wish_dir, float accel, float wish_speed)
{
  float current_speed = vec3_dot(p->velocity, wish_dir);
  float add_speed = wish_speed - current_speed;
  
  if (add_speed < 0)
    return;
  
  float accel_speed = accel * wish_speed * 0.015;
  
  if (accel_speed > wish_speed)
    accel_speed = add_speed;
  
  p->velocity = vec3_add(p->velocity, vec3_mulf(wish_dir, accel_speed));
}

void player_air_accelerate(player_t *p, vec3_t wish_dir, float wish_speed)
{
  float current_speed = vec3_dot(p->velocity, wish_dir);
  float add_speed = wish_speed - current_speed;
  
  if (add_speed < 0)
    return;
  
  p->velocity = vec3_add(p->velocity, vec3_mulf(wish_dir, add_speed));
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
