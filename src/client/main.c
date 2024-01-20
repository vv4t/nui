#include <stdio.h>
#include <stdbool.h>
#include "sdl.h"
#include "../common/log.h"
#include "../game/game.h"
#include "../game/map.h"
#include "../gl/gl.h"
#include "../renderer/renderer.h"
#include "../ngui/ngui.h"

#ifdef __EMSCRIPTEN__
  #include <emscripten.h>
#endif

static struct {
  game_t      game;
  usercmd_t   usercmd;
} nui;

bool nui_init();
void nui_update();

int main()
{
  if (!nui_init())
    return 1;
  
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(nui_update, 0, true);
#else
  while (sdl_should_not_quit())
    nui_update();
#endif
  
  sdl_quit();
  
  return 0;
}

bool nui_init()
{
  if (!sdl_init(1280, 720, "nui")) {
    return false;
  }
  
  if (!gl_init()) {
    return false;
  }
  
  nui.usercmd = (usercmd_t) {0};
  game_init(&nui.game);
  sdl_lock(true);
  
  if (!renderer_init(&nui.game)) {
    return false;
  }
  
  if (!ngui_init(1280, 720)) {
    return false;
  }
  
  map_t *map = map_load("nk_test");
  
  game_map_load(&nui.game, map);
  renderer_map_load(map);
  
  map_free(map);
  
  return true;
}

void nui_update()
{
  sdl_poll(&nui.usercmd);
  game_update(&nui.game, &nui.usercmd);
  renderer_render();
  sdl_swap();
}
