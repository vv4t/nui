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
  
  window_t window(WIDTH, HEIGHT, "nui", input);
  window.set_cursor_lock(true);
  
  game_t game;
  
  {
    entity_t entity = game.add_entity();
    transform_t& transform = game.enable_transform(entity, transform_t());
      transform.move_to(vec3(-2.0, 0.0, 0.0));
      transform.rotate_to(vec3(0.0, 0.4, 0.0));
    game.enable_model(entity, model_t());
  }
  
  {
    entity_t e = game.add_entity();
    transform_t& transform = game.enable_transform(e, transform_t());
      transform.move_to(vec3(+2.0, 0.0, 0.0));
    game.enable_model(e, model_t());
  }
  
  renderer_t renderer(game);
  renderer.bind();
  
  while (window.poll()) {
    glClear(GL_COLOR_BUFFER_BIT);
    game.update(input);
    renderer.render();
    window.swap();
  }
  
  return 0;
}
