#include "dither.h"

#include "frame.h"
#include "shader.h"

typedef struct {
  GLuint shader;
  frame_t frame;
} dither_t;

static dither_t dither;

bool dither_init(int width, int height)
{
  if (!fx_shader_load(&dither.shader, "dither", "")) {
    return false;
  }
  
  if (!frame_new(&dither.frame, dither.shader, width, height)) {
    return false;
  }
  
  return true;
}

void dither_begin()
{
  frame_begin(&dither.frame);
}

void dither_end()
{
  frame_end();
}

void dither_draw(int x, int y, int width, int height)
{
  frame_draw(&dither.frame, x, y, width, height);
}
