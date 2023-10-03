#ifndef BSP_H
#define BSP_H

#include "../common/nui_math.h"

typedef struct {
  vec3_t normal;
  float distance;
} bsp_plane_t;

typedef struct {
  bsp_plane_t plane;
  int behind;
  int ahead;
  int bevel;
} bsp_node_t;

typedef struct {
  bsp_node_t *nodes;
  int num_nodes;
} bsp_t;

typedef struct {
  vec3_t normal;
  float distance;
} clip_t;

typedef struct {
  vec3_t pos;
  float radius;
} sphere_t;

int bsp_clip_sphere(clip_t clips[], const bsp_t *bsp, const sphere_t *sphere);

#endif
