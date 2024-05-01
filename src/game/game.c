#include <game/game.h>

void game_init(game_t *g)
{
  
}

void game_update(game_t *g, int key[], float mx, float my)
{
  float speed = 0.1;
  vector walk = vec3(0, 0, 0);
  
  walk.z += key['w'] * speed;
  walk.z -= key['s'] * speed;
  walk.x -= key['a'] * speed;
  walk.x += key['d'] * speed;
  
  walk = mdotv(rotate_xyz(g->rotation), walk);
  
  g->rotation = vec3(-my * 4.0, -mx * 4.0, 0);
  g->position = vaddv(g->position, walk);
}
