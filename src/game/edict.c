#include <game/edict.h>

void edict_init(edict_t *edict)
{
  *edict = (edict_t) {0};
  edict->num_entities = 0;
}

entity_t edict_add(edict_t *edict)
{
  entity_t entity = INVALID_ENTITY;
  
  for (int i = 0; i < edict->num_entities; i++) {
    if (edict->field[i] == 0) {
      entity = i;
      break;
    }
  }
  
  if (entity == INVALID_ENTITY) {
    entity = edict->num_entities++;
  }
  
  return entity;
}
