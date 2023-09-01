#ifndef MDL_FILE_H
#define MDL_FILE_H

#include "../common/nui_math.h"

typedef struct {
  char diffuse[32];
} mdl_material_t;

typedef struct {
  mdl_material_t material;
  int offset;
  int count;
} mdl_vertex_group_t;

typedef struct {
  vec3_t pos;
  vec3_t normal;
  vec2_t uv;
} mdl_vertex_t;

typedef struct {
  int                 num_vertex_groups;
  mdl_vertex_group_t  *vertex_groups;
  
  int                 num_vertices;
  mdl_vertex_t        *vertices;
} mdl_file_t;

mdl_file_t  *mdl_file_load(const char *path);
void        mdl_file_free(mdl_file_t *mesh_file);

#endif
