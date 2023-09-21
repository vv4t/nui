#ifndef PLAYER_H
#define PLAYER_H

#include "../common/nui_math.h"
#include "usercmd.h"
#include "bsp.h"

typedef struct {
  vec3_t  position;
  vec3_t  velocity;
  quat_t  rotation;
  
  float   yaw;
  float   pitch;
} player_t;

void player_init(player_t *player);
void player_move(player_t *player, const bsp_t *bsp, const usercmd_t *usercmd);
void player_look(player_t *player, const usercmd_t *usercmd);

#endif
