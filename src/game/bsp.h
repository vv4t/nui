#ifndef BSP_H
#define BSP_H

#include "../common/nui_math.h"

#define MAX_HULL_VERTICES 32
#define MAX_CLIPS 8

typedef struct {
  plane_t plane;
  int behind;
  int ahead;
  int bevel;
} bsp_node_t;

typedef struct {
  bsp_node_t *nodes;
  int num_nodes;
} bsp_t;

typedef struct {
  float depth;
  vec3_t normal;
} clip_t;

typedef struct {
  clip_t clips[MAX_CLIPS];
  int num_clips;
} trace_t;

typedef struct {
  vec3_t pos;
  vec3_t vertices[MAX_HULL_VERTICES];
  int num_vertices;
} hull_t;

void bsp_clip_hull(trace_t *trace, const bsp_t *bsp, const hull_t *hull);

#endif
