#ifndef EDICT_H
#define EDICT_H

#define MAX_ENTITIES 64
#define INVALID_ENTITY -1

#include <game/transform.h>
#include <game/meshinstance.h>

typedef int entity_t;

typedef enum {
  C_transform     = 1 << 0,
  C_meshinstance  = 1 << 1
} component_t;

typedef struct {
  transform_t transform[MAX_ENTITIES];
  meshinstance_t meshinstance[MAX_ENTITIES];
  component_t field[MAX_ENTITIES];
  int num_entities;
} edict_t;

void edict_init(edict_t *edict);
entity_t edict_add(edict_t *edict);

#define ENTITY_ADD_COMPONENT(edict, entity, component) \
{\
  edict.component[entity] = create_##component();\
  edict.field[entity] |= C_##component;\
}

#define ENTITY_GET_COMPONENT(edict, entity, component) (&edict.component[entity])
#define ENTITY_MATCH(edict, entity, mask) ((edict.field[entity] & (mask)) != (mask))

#endif
