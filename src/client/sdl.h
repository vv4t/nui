#ifndef SDL_H
#define SDL_H

#include "../game/game.h"

bool sdl_init(int width, int height, const char *title);
void sdl_poll(usercmd_t *usercmd);
void sdl_lock(bool set_lock);
void sdl_swap();
void sdl_quit();
bool sdl_should_not_quit();

#endif
