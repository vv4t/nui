#ifndef BSP_FILE_H
#define BSP_FILE_H

#include "../common/nui_math.h"

typedef struct {
  vec3_t normal;
  float distance;
} bsp_plane_t;

typedef struct {
  bsp_plane_t plane;
  int behind;
  int ahead;
} bsp_node_t;

typedef struct {
  bsp_node_t *nodes;
  int num_nodes;
} bsp_file_t;

bsp_file_t  *bsp_file_load(const char *path);
void        bsp_file_free(bsp_file_t *bsp_file);

#endif
