#ifndef HULL_H
#define HULL_H

#include "../common/nui_math.h"

#define MAX_VERTICES 32
#define MAX_PLANES 32

typedef struct {
  vec3_t pos;
  
  int num_vertices;
  vec3_t vertices[MAX_VERTICES];
  
  int num_planes;
  plane_t planes[MAX_PLANES];
} hull_t;

vec3_t hull_furthest_in(const hull_t *hull, vec3_t d);
plane_t hull_calc_plane(const hull_t *hull, int plane_num);

#endif
