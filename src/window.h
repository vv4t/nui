#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include "input.h"

class window_t {
private:
  SDL_Window *m_window;
  SDL_GLContext m_gl;
  input_t& m_input;
  int m_width;
  int m_height;
  int m_mouse_x;
  int m_mouse_y;
  bool m_is_cursor_lock;

public:
  window_t(int width, int height, const char *title, input_t& input);
  ~window_t();
  bool poll();
  void swap();
  void cursor_lock(bool state);
};

#endif
