#include <iostream>
#include <glad/glad.h>
#include <vector>
#include "input.h"
#include "window.h"
#include "renderer.h"

#define WIDTH 800
#define HEIGHT 600

int main() {
  input_t input;
  input.bind_move(0, 1);
  
  window_t window(WIDTH, HEIGHT, "nui", input);
  window.set_cursor_lock(true);
  
  renderer_t renderer;
  renderer.bind();
  
  while (window.poll()) {
    glClear(GL_COLOR_BUFFER_BIT);
    renderer.render(input);
    window.swap();
  }
  
  return 0;
}
