#include "hdr.h"

#include "frame.h"

typedef struct {
  frame_t frame;
} hdr_t;

static hdr_t hdr;

bool hdr_init(int width, int height)
{
  if (!frame_new(&hdr.frame, "hdr", width, height)) {
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

void hdr_draw(int x, int y, int width, int height)
{
  frame_draw(&hdr.frame, x, y, width, height);
}
