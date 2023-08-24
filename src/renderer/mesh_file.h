#ifndef MESH_FILE_H
#define MESH_FILE_H

#include "buffer.h"

typedef struct {
  vertex_t  *vertices;
  int       num_vertices;
} mesh_file_t;

bool  mesh_file_load(mesh_file_t *mesh_file, const char *path);
void  mesh_file_free(mesh_file_t *mesh_file);

#endif
