#ifndef BSP_H
#define BSP_H

#include "../common/nui_math.h"

#define MAX_HULL_VERTICES 32

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
  vec3_t vertices[MAX_HULL_VERTICES];
  int num_vertices;
} hull_t;

int bsp_clip_hull(clip_t clips[], const bsp_t *bsp, const hull_t *hull);

float hull_furthest_in(const hull_t *hull, vec3_t direction);
float hull_furthest_out(const hull_t *hull, vec3_t direction);

#endif
