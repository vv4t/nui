#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"
#include "material.h"
#include "../game/map.h"

#define MAX_MESH_GROUPS 32

typedef struct {
  material_t  material;
  mesh_t      mesh;
} mesh_group_t;

typedef struct {
  int           num_meshes;
  mesh_group_t  mesh_groups[MAX_MESH_GROUPS];
} model_t;

bool model_load(model_t *model, const char *path);
bool model_load_map(model_t *model, const map_t *map);
void model_draw(const model_t *model);

#endif
