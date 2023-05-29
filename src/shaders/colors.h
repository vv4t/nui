#ifndef COLORS_H
#define COLORS_H

#include "../renderer/gl.h"
#include "../common/nui_math.h"

typedef struct {
  GLuint  shader;
  GLuint  ul_color;
} colors_t;

bool colors_init(colors_t *color);
void colors_bind(colors_t *color);
void colors_set_color(colors_t *colors, vec4_t color);

#endif
