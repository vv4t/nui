#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"
#include "material.h"
#include "../game/map_file.h"

#define MAX_MESH_GROUPS 32

typedef struct {
  material_t  material;
  mesh_t      mesh;
} mesh_group_t;

typedef struct {
  int           num_meshes;
  mesh_group_t  mesh_groups[MAX_MESH_GROUPS];
} model_t;

bool model_load(model_t *model, mesh_buffer_t *mesh_buffer, const char *path);
bool model_load_map(model_t *model, mesh_buffer_t *mesh_buffer, map_file_t *map_file);
void model_draw(const model_t *model);

#endif
