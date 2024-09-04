#include "window.h"
#include <glad/glad.h>
#include <iostream>

void window_t::init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "error: SDL_Init: " << SDL_GetError();
    throw std::runtime_error("failed to initialise SDL");
  }
}

window_t::window_t(int width, int height, const char *title) {
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
    case SDL_TEXTINPUT:
    case SDL_KEYUP:
    case SDL_KEYDOWN:
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEMOTION:
      break;
    }
  }
  
  return true;
}

window_t::~window_t() {
  SDL_GL_DeleteContext(m_gl);
  SDL_DestroyWindow(m_window);
  SDL_Quit();
}
