#ifndef PLAYER_H
#define PLAYER_H

#include "bsp.h"
#include "hull.h"
#include "usercmd.h"
#include "../common/nui_math.h"
#include <stdbool.h>

typedef struct {
  vec3_t  position;
  vec3_t  velocity;
  quat_t  rotation;
  
  float   yaw;
  float   pitch;
  
  bool    ground;
  
  hull_t  hull;
} player_t;

void player_init(player_t *player);
void player_update(player_t *p, const bsp_t *bsp, const usercmd_t *usercmd);

#endif
