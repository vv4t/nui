#ifndef GAME_H
#define GAME_H

#include <math3d.h>

typedef struct {
  vector position;
  vector rotation;
} game_t;

void game_init(game_t *g);
void game_update(game_t *g, int key[], float mx, float my);

#endif
