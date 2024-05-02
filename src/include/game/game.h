#ifndef GAME_H
#define GAME_H

#include <game/edict.h>

typedef struct {
  edict_t edict;
  entity_t player;
  float time;
} game_t;

void game_init(game_t *game);
void game_update(game_t *game, int key[], float mx, float my);

#endif
