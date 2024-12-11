#include <iostream>
#include <glad/glad.h>
#include <vector>
#include "input.hpp"
#include "window.hpp"
#include <renderer/renderer.hpp>

#define WIDTH 800
#define HEIGHT 600

void add_cuboid(game_t& game, vec3 a, vec3 b) {
  entity_t e = game.add_entity();
  transform_t& transform = game.enable_transform(e, transform_t());
    transform.move_to(a);
    transform.scale_to(b);
  game.enable_aabb(e, aabb_t(vec3(), b));
  game.enable_model(e, model_t(MESHNAME_CUBOID));
}

int WinMain() {
  input_t input;
  input.bind_move(0, 1);
  input.bind_key(2, 'w');
  input.bind_key(3, 'a');
  input.bind_key(4, 's');
  input.bind_key(5, 'd');
  input.bind_key(6, ' ');
  
  window_t window(WIDTH, HEIGHT, "nui", input);
  window.set_cursor_lock(true);
  
  game_t game;

  add_cuboid(game, vec3(0.0, -1.0, 0.0), vec3(40.0, 1.0, 40.0));
  add_cuboid(game, vec3(5.0, 0.0, 5.0), vec3(5.0, 2.0, 4.0));

  add_cuboid(game, vec3(2.0, 0.0, 5.0), vec3(3.0, 0.5, 1.0));
  add_cuboid(game, vec3(2.0, 0.0, 6.0), vec3(3.0, 1.0, 1.0));
  add_cuboid(game, vec3(2.0, 0.0, 7.0), vec3(3.0, 1.5, 1.0));
  add_cuboid(game, vec3(2.0, 0.0, 8.0), vec3(3.0, 2.0, 1.0));
  
  renderer_t renderer(game);
  renderer.bind();
  
  glEnable(GL_DEPTH_TEST);
  
  glClearColor(0.5, 0.5, 1.0, 1.0);

  int old_time = window.get_time();
  int lag_time = 0;
  
  while (window.poll()) {
    if (lag_time > 0) {
      lag_time -= 15;
      game.update(input);
      renderer.render();
      window.swap();
    }

    int now_time = window.get_time();
    lag_time += now_time - old_time;
    old_time = now_time;
  }
  
  return 0;
}
