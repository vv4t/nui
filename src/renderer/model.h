#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"
#include "material.h"
#include "../game/map.h"

#define MAX_SUBGROUPS 32

typedef struct {
  material_t material;
  mesh_t mesh;
} subgroup_t;

typedef struct {
  int num_subgroups;
  subgroup_t subgroups[MAX_SUBGROUPS];
} model_t;

bool model_load(model_t *model, const char *path);
bool model_load_map(model_t *model, const map_t *map);
void model_draw(const model_t *model);

#endif
