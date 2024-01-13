#include "map.h"

#include "../common/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH_LEN 256

map_t *map_load(const char *name)
{
  map_t *map = malloc(sizeof(map_t));
  
  path_create(map->path, "assets/map/%s/%s.map", name, name);
  
  FILE *file = fopen(map->path, "rb");
  
  if (!file) {
    LOG_ERROR("failed to open '%s'", map->path);
    return NULL;
  }
  
  fread(&map->num_vertices, sizeof(int), 1, file); 
  fread(&map->num_planes, sizeof(int), 1, file); 
  fread(&map->num_faces, sizeof(int), 1, file); 
  fread(&map->num_hulls, sizeof(int), 1, file); 
  fread(&map->num_nodes, sizeof(int), 1, file); 
  fread(&map->num_subgroups, sizeof(int), 1, file); 
  fread(&map->num_lights, sizeof(int), 1, file); 
  
  map->vertices = calloc(map->num_vertices, sizeof(map_vertex_t));
  map->planes = calloc(map->num_planes, sizeof(plane_t));
  map->faces = calloc(map->num_faces, sizeof(map_face_t));
  map->hulls = calloc(map->num_hulls, sizeof(map_hull_t));
  map->nodes = calloc(map->num_nodes, sizeof(map_bsp_node_t));
  map->subgroups = calloc(map->num_subgroups, sizeof(map_subgroup_t));
  map->lights = calloc(map->num_lights, sizeof(map_light_t));
  
  fread(map->vertices, sizeof(map_vertex_t), map->num_vertices, file);
  fread(map->planes, sizeof(plane_t), map->num_planes, file);
  fread(map->faces, sizeof(map_face_t), map->num_faces, file);
  fread(map->hulls, sizeof(map_hull_t), map->num_hulls, file);
  fread(map->nodes, sizeof(map_bsp_node_t), map->num_nodes, file);
  fread(map->subgroups, sizeof(map_subgroup_t), map->num_subgroups, file);
  fread(map->lights, sizeof(map_light_t), map->num_lights, file);
  
  return map;
}

void map_free(map_t *map)
{
  free(map->vertices);
  free(map->planes);
  free(map->hulls);
  free(map->nodes);
  free(map->subgroups);
  free(map->lights);
  free(map);
}
