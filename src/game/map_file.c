#include "map_file.h"

#include "../common/log.h"
#include <stdio.h>
#include <stdlib.h>

map_file_t *map_file_load(const char *path)
{
  FILE *file = fopen(path, "rb");
  
  if (!file) {
    LOG_ERROR("failed to open '%s'", path);
    return NULL;
  }
  
  map_file_t *map_file = malloc(sizeof(map_file_t));
  
  fread(&map_file->num_nodes, sizeof(int), 1, file); 
  fread(&map_file->num_vertex_groups, sizeof(int), 1, file); 
  fread(&map_file->num_vertices, sizeof(int), 1, file); 
  
  map_file->nodes = calloc(map_file->num_nodes, sizeof(map_bsp_node_t));
  map_file->vertex_groups = calloc(map_file->num_vertex_groups, sizeof(map_vertex_group_t));
  map_file->vertices = calloc(map_file->num_vertices, sizeof(map_vertex_t));
  
  fread(map_file->nodes, sizeof(map_bsp_node_t), map_file->num_nodes, file);
  fread(map_file->vertex_groups, sizeof(map_vertex_group_t), map_file->num_vertex_groups, file);
  fread(map_file->vertices, sizeof(map_vertex_t), map_file->num_vertices, file);
  
  return map_file;
}

bsp_t *map_load_bsp(map_file_t *map_file)
{
  bsp_t *bsp = malloc(sizeof(bsp_t));
  bsp->nodes = calloc(map_file->num_nodes, sizeof(map_bsp_node_t));
  
  for (int i = 0; i < map_file->num_nodes; i++) {
    bsp->nodes[i].plane.normal = map_file->nodes[i].plane.normal;
    bsp->nodes[i].plane.distance = map_file->nodes[i].plane.distance;
    bsp->nodes[i].behind = map_file->nodes[i].behind;
    bsp->nodes[i].ahead = map_file->nodes[i].ahead;
  }
  
  return bsp;
}

void map_file_free(map_file_t *map_file)
{
  free(map_file->vertices);
  free(map_file->vertex_groups);
  free(map_file->nodes);
  free(map_file);
}
