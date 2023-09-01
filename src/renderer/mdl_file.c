#include "mdl_file.h"

#include "../common/log.h"
#include <stdio.h>
#include <stdlib.h>

mdl_file_t *mdl_file_load(const char *path)
{
  FILE *file = fopen(path, "rb");
  if (!file) {
    LOG_ERROR("failed to open '%s'", path);
    return NULL;
  }
  
  mdl_file_t *mdl_file = malloc(sizeof(mdl_file_t));
  
  fread(&mdl_file->num_vertex_groups, sizeof(int), 1, file); 
  fread(&mdl_file->num_vertices, sizeof(int), 1, file); 
  
  mdl_file->vertex_groups = calloc(mdl_file->num_vertex_groups, sizeof(mdl_vertex_group_t));
  mdl_file->vertices = calloc(mdl_file->num_vertices, sizeof(mdl_vertex_t));
  
  fread(mdl_file->vertex_groups, sizeof(mdl_vertex_group_t), mdl_file->num_vertex_groups, file);
  fread(mdl_file->vertices, sizeof(mdl_vertex_t), mdl_file->num_vertices, file);
  
  return mdl_file;
}

void mdl_file_free(mdl_file_t *mdl_file)
{
  free(mdl_file->vertex_groups);
  free(mdl_file->vertices);
  free(mdl_file);
}
