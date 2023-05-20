#ifndef SDL_H
#define SDL_H

#include <stdbool.h>
#include "common/log.h"
#include "game/game.h"
#include <SDL2/SDL.h>

typedef struct {
  SDL_Window    *window;
  SDL_GLContext gl_context;
  bool          quit;
} sdl_t;

bool sdl_init(sdl_t *sdl, int width, int height, const char *title);
void sdl_poll(sdl_t *sdl, usercmd_t *usercmd);
void sdl_lock(bool set_lock);
void sdl_swap(sdl_t *sdl);
void sdl_quit(sdl_t *sdl);

#endif
