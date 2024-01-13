#ifndef MAP_H
#define MAP_H

#include "../common/nui_math.h"
#include "../common/path.h"

typedef struct {
  vec3_t pos;
} map_vertex_t;

typedef struct {
  int vertices[3];
  vec3_t normal;
} map_face_t;

typedef struct {
  int vertex_offset;
  int vertex_count;
  int plane_offset;
  int plane_count;
} map_hull_t;

typedef struct {
  int plane;
  int behind;
  int ahead;
  int hull;
} map_bsp_node_t;

typedef struct {
  char diffuse[64];
  char normal[64];
  vec3_t color;
  float specular;
} map_material_t;

typedef struct {
  map_material_t material;
  int face_offset;
  int face_count;
} map_subgroup_t;

typedef struct {
  vec3_t pos;
  vec3_t color;
  float intensity;
} map_light_t;

typedef struct {
  path_t          path;
  
  int             num_vertices;
  map_vertex_t    *vertices;
  
  int             num_planes;
  plane_t         *planes;
  
  int             num_faces;
  map_face_t      *faces;
  
  int             num_hulls;
  map_hull_t      *hulls;
  
  int             num_nodes;
  map_bsp_node_t  *nodes;
  
  int             num_subgroups;
  map_subgroup_t  *subgroups;
  
  int             num_lights;
  map_light_t     *lights;
} map_t;

map_t *map_load(const char *path);
void  map_free(map_t *map);

#endif
