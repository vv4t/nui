#ifndef WAVE_H
#define WAVE_H

#include "../gl/gl.h"
#include <stdbool.h>

typedef struct {
  GLuint fbo[2];
  GLuint buffer[2];
  GLuint normal;
  int turn;
  int width;
  int height;
} wave_t;

bool wave_init();
void wave_new(wave_t *wave, int width, int height);
void wave_update(wave_t *wave);

#endif
