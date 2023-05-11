#ifndef GAME_H
#define GAME_H

#include "../common/nui_math.h"

#define GAME_SENSITIVITY 0.01

typedef struct {
  int   forward;
  int   back;
  int   left;
  int   right;
  
  float d_yaw;
  float d_pitch;
} usercmd_t;

typedef struct {
  vec3_t  position;
  quat_t  rotation;
  float   time;
  
  float   camera_yaw;
  float   camera_pitch;
} game_t;

void game_init(game_t *game);
void game_update(game_t *game, float delta, const usercmd_t *usercmd);

#endif
