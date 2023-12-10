#include "hdr.h"

#include "frame.h"
#include "shader.h"

typedef struct {
  GLuint shader;
  frame_t frame;
} hdr_t;

static hdr_t hdr;

bool hdr_init(int width, int height)
{
  if (!fx_shader_load(&hdr.shader, "hdr", "")) {
    return false;
  }
  
  if (!frame_new(&hdr.frame, hdr.shader, width, height)) {
    return false;
  }
  
  return true;
}

void hdr_begin()
{
  frame_begin(&hdr.frame);
}

void hdr_end()
{
  frame_end();
}

void hdr_draw()
{
  frame_draw(&hdr.frame);
}
