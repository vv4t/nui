#include <stdio.h>
#include "common/log.h"
#include "game/game.h"
#include "renderer/gl.h"
#include "renderer/renderer.h"

typedef struct {
  SDL_Window    *window;
  SDL_GLContext gl_context;
} sdl_t;

bool sdl_init(sdl_t *sdl, int width, int height, const char *title);
bool sdl_poll(usercmd_t *usercmd);
void sdl_lock(bool set_lock);
void sdl_swap(sdl_t *sdl);
void sdl_quit(sdl_t *sdl);
void key_event(usercmd_t *usercmd, int key, int action);
void mouse_move(usercmd_t *usercmd, int d_x, int d_y);

int main()
{
  sdl_t sdl;
  if (!sdl_init(&sdl, 1280, 720, "nui"))
    return 0;
  
  sdl_lock(true);
  
  if (!gl_init())
    return 0;
  
  renderer_t renderer;
  if (!renderer_init(&renderer))
    return 0;
  
  usercmd_t usercmd = {0};
  
  game_t game;
  game_init(&game);
  
  while (sdl_poll(&usercmd)) {
    game_update(&game, 0.015, &usercmd);
    renderer_render(&renderer, &game);
    sdl_swap(&sdl);
  }
  
  sdl_quit(&sdl);
  
  return 0;
}

void sdl_lock(bool set_lock)
{
  SDL_SetRelativeMouseMode(set_lock);
}

bool sdl_poll(usercmd_t *usercmd)
{
  usercmd->d_yaw = 0;
  usercmd->d_pitch = 0;
  
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      return false;
    case SDL_KEYUP:
      key_event(usercmd, event.key.keysym.sym, 0);
      break;
    case SDL_KEYDOWN:
      key_event(usercmd, event.key.keysym.sym, 1);
      break;
    case SDL_MOUSEMOTION:
      mouse_move(usercmd, event.motion.xrel, event.motion.yrel);
      break;
    }
  }
  
  return true;
}

void key_event(usercmd_t *usercmd, int key, int action)
{
  if (key == 'a')
    usercmd->left = action;
  if (key == 'd')
    usercmd->right = action;
  if (key == 'w')
    usercmd->forward = action;
  if (key == 's')
    usercmd->back = action;
}

void mouse_move(usercmd_t *usercmd, int d_x, int d_y)
{
  usercmd->d_pitch = d_y;
  usercmd->d_yaw = d_x;
}

bool sdl_init(sdl_t *sdl, int width, int height, const char *title)
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  
  sdl->window = SDL_CreateWindow(
    title,
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    width,
    height,
    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  
  if (!sdl->window) {
    LOG_ERROR("failed to create SDL window");
    return false;
  }
  
  sdl->gl_context = SDL_GL_CreateContext(sdl->window);
  
  if (!sdl->gl_context) {
    LOG_ERROR("failed to create GL context");
    return false;
  }
  
  return true;
}

void sdl_swap(sdl_t *sdl)
{
  SDL_GL_SwapWindow(sdl->window);
}

void sdl_quit(sdl_t *sdl)
{
  SDL_GL_DeleteContext(sdl->gl_context);
  SDL_DestroyWindow(sdl->window);
  SDL_Quit();
}
