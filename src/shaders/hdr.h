#ifndef HDR_H
#define HDR_H

#include "../ref/gl.h"
#include "../ref/ref_def.h"
#include <stdbool.h>

typedef struct {
  mesh_t  quad_mesh;
  
  GLuint  shader;
  
  GLuint  fbo;
  GLuint  rbo;
  
  GLuint  color_buffer;
} hdr_t;

bool hdr_init(hdr_t *hdr, mesh_t quad_mesh);
void hdr_begin(hdr_t *hdr);
void hdr_end(hdr_t *hdr);
void hdr_draw(hdr_t *hdr);

#endif
