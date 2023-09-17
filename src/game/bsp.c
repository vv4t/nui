#include "bsp.h"

#include "../common/log.h"
#include <stdio.h>
#include <stdlib.h>

bsp_file_t *bsp_file_load(const char *path)
{
  FILE *file = fopen(path, "rb");
  if (!file) {
    LOG_ERROR("failed to open '%s'", path);
    return NULL;
  }
  
  bsp_file_t *bsp_file = malloc(sizeof(bsp_file_t));
  
  fread(&bsp_file->num_nodes, sizeof(int), 1, file); 
  
  bsp_file->nodes = calloc(bsp_file->num_nodes, sizeof(bsp_node_t));
  fread(bsp_file->nodes, sizeof(bsp_node_t), bsp_file->num_nodes, file);
  
  return bsp_file;
}

void bsp_file_free(bsp_file_t *bsp_file)
{
  free(bsp_file->nodes);
  free(bsp_file);
}
