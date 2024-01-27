#include "player.h"

#define LOOK_SENSITIVITY 0.0035

void player_move(player_t *player, const bsp_t *bsp, const usercmd_t *usercmd);
void player_free_move(player_t *p, const bsp_t *bsp, const usercmd_t *usercmd);
void player_look(player_t *player, const usercmd_t *usercmd);
vec3_t player_move_dir(const player_t *p, const usercmd_t *usercmd);
void player_slide(player_t *p, const bsp_t *bsp);
void player_accelerate(player_t *p, vec3_t wish_dir, float accel, float wish_speed);
void player_air_accelerate(player_t *p, vec3_t wish_dir, float wish_speed);

void player_init(player_t *p)
{
  p->position = vec3_init(0.0, 1.0, 0.0);
  p->rotation = quat_init(0.0, 0.0, 0.0, 1.0);
  
  p->yaw = 0.0;
  p->pitch = 0.0;
  
  float w = 0.4;
  float h = 1.7;
  float l = 0.1;
  
  p->hull = (hull_t) {
    .pos = { .x = 0.0, .y = 0.0, .z = 0.0 },
    .vertices = {
      { .x = -w, .y = +l, .z = -w },
      { .x = +w, .y = +l, .z = -w },
      { .x = -w, .y = +l, .z = +w },
      { .x = +w, .y = +l, .z = +w },
      { .x = -w, .y = -h, .z = -w },
      { .x = +w, .y = -h, .z = -w },
      { .x = -w, .y = -h, .z = +w },
      { .x = +w, .y = -h, .z = +w }
    },
    .num_vertices = 8,
    .planes = {
      { .normal = { .x =  1.0, .y =  0.0, .z =  0.0 }, .distance = w },
      { .normal = { .x =  0.0, .y =  1.0, .z =  0.0 }, .distance = l },
      { .normal = { .x =  0.0, .y =  0.0, .z =  1.0 }, .distance = w },
      { .normal = { .x = -1.0, .y =  0.0, .z =  0.0 }, .distance = w },
      { .normal = { .x =  0.0, .y = -1.0, .z =  0.0 }, .distance = h },
      { .normal = { .x =  0.0, .y =  0.0, .z = -1.0 }, .distance = w }
    },
    .num_planes = 6
  };
}

void player_update(player_t *p, const bsp_t *bsp, const usercmd_t *usercmd)
{
  player_look(p, usercmd);
  player_move(p, bsp, usercmd);
}

void player_free_move(player_t *p, const bsp_t *bsp, const usercmd_t *usercmd)
{
  vec3_t cmd_dir = vec3_init(usercmd->right - usercmd->left, 0.0f, usercmd->forward - usercmd->back);
  vec3_t wish_dir = vec3_rotate(cmd_dir, p->rotation);
  vec3_t move_dir = vec3_normalize(wish_dir);
  
  vec3_t delta_pos = vec3_mulf(move_dir, 0.05);
  
  p->position = vec3_add(p->position, delta_pos);
  p->hull.pos = p->position;
  
  trace_t trace;
  bsp_clip(&trace, bsp, &p->hull);
  
  p->position = p->hull.pos;
}

void player_move(player_t *p, const bsp_t *bsp, const usercmd_t *usercmd)
{
  vec3_t move_dir = player_move_dir(p, usercmd);
  
  if (p->ground) {
    player_accelerate(p, move_dir, 6.0, 13.0);
    p->velocity = vec3_mulf(p->velocity, 0.8f);
    
    if (usercmd->jump) {
      p->velocity.y += 7.0f;
    }
  } else {
    p->velocity.y -= 17.8 * 0.015;
    player_air_accelerate(p, move_dir, 1.3);
  }
  
  p->position = vec3_add(p->position, vec3_mulf(p->velocity, 0.015));
  p->hull.pos = p->position;
  
  player_slide(p, bsp);
}

void player_slide(player_t *p, const bsp_t *bsp)
{
  p->ground = false;
  
  trace_t trace;
  bsp_clip(&trace, bsp, &p->hull);
  
  for (int i = 0; i < trace.num_clips; i++) {
    float lambda = -vec3_dot(p->velocity, trace.clips[i].plane.normal);
    
    if (lambda > 0) {
      p->velocity = vec3_add(p->velocity, vec3_mulf(trace.clips[i].plane.normal, lambda));
    }
    
    if (trace.clips[i].plane.normal.y > 0.4) {
      p->ground = true;
    }
  }
  
  p->position = p->hull.pos;
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
