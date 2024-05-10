#ifndef SKYBOX_H
#define SKYBOX_H

#include <renderer/texture.h>

void skybox_init();
void skybox_set_cube_map(texture_t cube_map);
void skybox_draw();
void skybox_deinit();

#endif
