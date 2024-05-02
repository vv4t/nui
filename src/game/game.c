#include <game/game.h>

void game_init(game_t *gs)
{
  *gs = (game_t) {0};
  gs->time = 0.0;
  
  gs->player = edict_add(&gs->edict);
  ENTITY_ADD_COMPONENT(gs->edict, gs->player, transform);
  
  transform_t *t;
  meshinstance_t *m;
  
  {
    entity_t e = edict_add(&gs->edict);
    ENTITY_ADD_COMPONENT(gs->edict, e, transform);
    ENTITY_ADD_COMPONENT(gs->edict, e, meshinstance);
    t = ENTITY_GET_COMPONENT(gs->edict, e, transform);
    m = ENTITY_GET_COMPONENT(gs->edict, e, meshinstance);
    t->position = vec3(0, -1, 0);
    t->scale = vec3(10, 0, 10);
    m->meshname = MESH_PLANE;
  }
  
  {
    entity_t e = edict_add(&gs->edict);
    ENTITY_ADD_COMPONENT(gs->edict, e, transform);
    ENTITY_ADD_COMPONENT(gs->edict, e, meshinstance);
    t = ENTITY_GET_COMPONENT(gs->edict, e, transform);
    m = ENTITY_GET_COMPONENT(gs->edict, e, meshinstance);
    t->position = vec3(0, 0, 3);
    m->meshname = MESH_CUBE;
  }
}

void game_update(game_t *gs, int key[], float mx, float my)
{
  transform_t *pt = ENTITY_GET_COMPONENT(gs->edict, gs->player, transform);
  
  float speed = 0.1;
  vector walk = vec3(0, 0, 0);
  
  walk.z += key['w'] * speed;
  walk.z -= key['s'] * speed;
  walk.x -= key['a'] * speed;
  walk.x += key['d'] * speed;
  
  walk = mdotv(rotate_xyz(pt->rotation), walk);
  
  pt->rotation = vec3(-my * 4.0, -mx * 4.0, 0);
  pt->position = vaddv(pt->position, walk);
  
  ENTITY_GET_COMPONENT(gs->edict, 2, transform)->position= vec3(cos(gs->time), 0, 0);
  
  gs->time += 0.015;
}
