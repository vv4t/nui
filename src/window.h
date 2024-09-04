#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>

class window_t {
private:
  SDL_Window *m_window;
  SDL_GLContext m_gl;

public:
  window_t(int width, int height, const char *title);
  ~window_t();
  bool poll();
  void swap();

  static void init();
};

#endif
