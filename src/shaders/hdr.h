#ifndef HDR_H
#define HDR_H

#include "../renderer/gl.h"
#include "../renderer/vertex_buffer.h"
#include "../renderer/scene.h"
#include <stdbool.h>

typedef struct {
  mesh_t  screen_mesh;
  
  GLuint  shader;
  
  GLuint  fbo;
  GLuint  rbo;
  
  GLuint  color_buffer;
} hdr_t;

bool hdr_init(hdr_t *hdr, vertex_buffer_t *vertex_buffer);
void hdr_begin(hdr_t *hdr);
void hdr_end(hdr_t *hdr);
void hdr_draw(hdr_t *hdr);

#endif
