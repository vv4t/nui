#ifndef BSP_FILE_H
#define BSP_FILE_H

#include "../common/nui_math.h"
#include "../common/path.h"
#include "bsp.h"

typedef struct {
  char diffuse[64];
} map_material_t;

typedef struct {
  map_material_t material;
  int offset;
  int count;
} map_vertex_group_t;

typedef struct {
  vec3_t pos;
  vec3_t normal;
  vec2_t uv;
} map_vertex_t;

typedef struct {
  vec3_t normal;
  float distance;
} map_plane_t;

typedef struct {
  plane_t plane;
  int behind;
  int ahead;
  int bevel;
} map_bsp_node_t;

typedef struct {
  path_t              path;
  
  map_bsp_node_t      *nodes;
  int                 num_nodes;
  
  int                 num_vertex_groups;
  map_vertex_group_t  *vertex_groups;
  
  int                 num_vertices;
  map_vertex_t        *vertices;
} map_t;

map_t *map_load(const char *path);
bsp_t *map_load_bsp(map_t *map);
void  map_free(map_t *map);

#endif
