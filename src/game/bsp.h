#ifndef BSP_H
#define BSP_H

#include "../common/nui_math.h"
#include "map.h"
#include "hull.h"

#define MAX_CLIPS 8

typedef struct {
  int num_vertices;
  map_vertex_t *vertices;
  
  int num_planes;
  plane_t *planes;
  
  int num_hulls;
  map_hull_t *hulls;
  
  int num_nodes;
  map_bsp_node_t *nodes;
} bsp_t;

typedef struct {
  plane_t plane;
  float depth;
} clip_t;

typedef struct {
  clip_t clips[MAX_CLIPS];
  int num_clips;
} trace_t;

void bsp_load(bsp_t *bsp, const map_t *map);
void bsp_clip(trace_t *trace, const bsp_t *bsp, const hull_t *hull);

#endif
