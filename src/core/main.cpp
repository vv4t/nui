#include <iostream>
#include <glad/glad.h>
#include <vector>
#include "input.hpp"
#include "window.hpp"
#include <renderer/renderer.hpp>

#define WIDTH 800
#define HEIGHT 600

int main() {
  input_t input;
  input.bind_move(0, 1);
  input.bind_key(2, 'w');
  input.bind_key(3, 'a');
  input.bind_key(4, 's');
  input.bind_key(5, 'd');
  
  window_t window(WIDTH, HEIGHT, "nui", input);
  window.set_cursor_lock(true);
  
  game_t game;
  
  {
    entity_t e = game.add_entity();
    transform_t& transform = game.enable_transform(e, transform_t());
      transform.move_to(vec3(0.0, 0.0, 0.0));
    game.enable_model(e, model_t());
  }
  
  renderer_t renderer(game);
  renderer.bind();
  
  glEnable(GL_DEPTH_TEST);
  
  glClearColor(0.5, 0.5, 1.0, 1.0);
  glDepthFunc(GL_LESS);
  
  while (window.poll()) {
    game.update(input);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderer.render();
    window.swap();
  }
  
  return 0;
}
