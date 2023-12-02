#include "player.h"

#define LOOK_SENSITIVITY 0.005

vec3_t player_move_dir(const player_t *p, const usercmd_t *usercmd);
void player_slide(player_t *p, const bsp_t *bsp);
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
  vec3_t move_dir = player_move_dir(p, usercmd);
  
  if (p->ground) {
    player_accelerate(p, move_dir, 4.0, 6.0);
    p->velocity = vec3_mulf(p->velocity, 0.9f);
    
    if (usercmd->jump) {
      p->velocity.y += 3.0f;
    }
  } else {
    p->velocity.y -= 9.8 * 0.015;
    player_air_accelerate(p, move_dir, 0.6);
  }
  
  p->position = vec3_add(p->position, vec3_mulf(p->velocity, 0.015));
  player_hull.pos = p->position;
  
  player_slide(p, bsp);
}

void player_slide(player_t *p, const bsp_t *bsp)
{
  p->ground = false;
  
  trace_t trace;
  bsp_clip_hull(&trace, bsp, &player_hull);
  
  for (int i = 0; i < trace.num_clips; i++) {
    float pos_shift = -trace.clips[i].depth;
    float vel_shift = -vec3_dot(p->velocity, trace.clips[i].normal);
    
    vec3_t pos_slide = vec3_mulf(trace.clips[i].normal, pos_shift);
    vec3_t vel_slide = vec3_mulf(trace.clips[i].normal, vel_shift);
    
    if (trace.clips[i].normal.y > 0.4) {
      p->ground = true;
    }
    
    if (vel_shift > 0) {
      p->velocity = vec3_add(p->velocity, vel_slide);
    }
    
    if (pos_shift > 0) {
      p->position = vec3_add(p->position, pos_slide);
    }
  }
}

vec3_t player_move_dir(const player_t *p, const usercmd_t *usercmd)
{
  vec3_t cmd_dir = vec3_init(usercmd->right - usercmd->left, 0.0f, usercmd->forward - usercmd->back);
  vec3_t wish_dir = vec3_rotate(cmd_dir, p->rotation);
  wish_dir.y = 0.0f;
  
  return wish_dir;
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
