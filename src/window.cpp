#include "window.h"
#include <glad/glad.h>
#include <iostream>

window_t::window_t(int width, int height, const char *title, input_t& input) : m_input(input) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "error: SDL_Init: " << SDL_GetError();
    throw std::runtime_error("failed to initialise SDL");
  }
  
  m_window = SDL_CreateWindow(
    title,
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    width,
    height,
    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
  );
  
  if (!m_window) {
    std::cerr << "error: SDL_CreateWindow: " << SDL_GetError();
    throw std::runtime_error("failed to create SDL window");
  }
  
  m_gl = SDL_GL_CreateContext(m_window);
  
  if (!m_gl) {
    std::cerr << "error: SDL_GL_CreateContext: " << SDL_GetError();
    throw std::runtime_error("failed to initialize SDL GL Context");
  }
  
  if (!gladLoadGLES2Loader((GLADloadproc) SDL_GL_GetProcAddress)) {
    throw std::runtime_error("failed to initialize GLAD");
  }
  
  m_width = width;
  m_height = height;
  m_mouse_x = 0;
  m_mouse_y = 0;
  m_is_cursor_lock = false;
}

void window_t::swap() {
  SDL_GL_SwapWindow(m_window);
}

bool window_t::poll() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      return false;
    case SDL_KEYUP:
      m_input.key_event(event.key.keysym.sym, false);
      break;
    case SDL_KEYDOWN:
      m_input.key_event(event.key.keysym.sym, true);
      break;
    case SDL_MOUSEMOTION:
      if (m_is_cursor_lock) {
        m_mouse_x += event.motion.xrel;
        m_mouse_y += event.motion.yrel;
      } else {
        m_mouse_x = event.motion.x;
        m_mouse_y = event.motion.y;
      }
      m_input.move_event(m_mouse_x / (float) m_width, 1.0 - m_mouse_y / (float) m_height);
      break;
    case SDL_TEXTINPUT:
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
      break;
    }
  }
  
  return true;
}

void window_t::cursor_lock(bool state) {
  SDL_SetRelativeMouseMode(state ? SDL_TRUE : SDL_FALSE);
  m_is_cursor_lock = state;
}

window_t::~window_t() {
  SDL_GL_DeleteContext(m_gl);
  SDL_DestroyWindow(m_window);
  SDL_Quit();
}
