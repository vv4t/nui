#include "map.h"

#include "../common/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH_LEN 256

map_t *map_load(const char *name)
{
  char path[PATH_LEN];
  strncpy(path, "assets/map/", PATH_LEN);
  strcat(path, name);
  strcat(path, "/");
  
  int base_len = strlen(path);
  
  strcat(path, name);
  strcat(path, ".map");
  
  FILE *file = fopen(path, "rb");
  
  if (!file) {
    LOG_ERROR("failed to open '%s'", path);
    return NULL;
  }
  
  map_t *map = malloc(sizeof(map_t));
  
  fread(&map->num_nodes, sizeof(int), 1, file); 
  fread(&map->num_vertex_groups, sizeof(int), 1, file); 
  fread(&map->num_vertices, sizeof(int), 1, file); 
  
  map->nodes = calloc(map->num_nodes, sizeof(map_bsp_node_t));
  map->vertex_groups = calloc(map->num_vertex_groups, sizeof(map_vertex_group_t));
  map->vertices = calloc(map->num_vertices, sizeof(map_vertex_t));
  
  fread(map->nodes, sizeof(map_bsp_node_t), map->num_nodes, file);
  fread(map->vertex_groups, sizeof(map_vertex_group_t), map->num_vertex_groups, file);
  fread(map->vertices, sizeof(map_vertex_t), map->num_vertices, file);
  
  return map;
}

bsp_t *map_load_bsp(map_t *map)
{
  bsp_t *bsp = malloc(sizeof(bsp_t));
  bsp->nodes = calloc(map->num_nodes, sizeof(map_bsp_node_t));
  
  for (int i = 0; i < map->num_nodes; i++) {
    bsp->nodes[i].plane.normal = map->nodes[i].plane.normal;
    bsp->nodes[i].plane.distance = map->nodes[i].plane.distance;
    bsp->nodes[i].behind = map->nodes[i].behind;
    bsp->nodes[i].ahead = map->nodes[i].ahead;
  }
  
  return bsp;
}

void map_free(map_t *map)
{
  free(map->vertices);
  free(map->vertex_groups);
  free(map->nodes);
  free(map);
}
