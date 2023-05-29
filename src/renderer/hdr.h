#ifndef HDR_H
#define HDR_H

#include "gl.h"
#include "renderer_def.h"
#include <stdbool.h>

typedef struct {
  mesh_t    quad_mesh;
  
  shader_t  shader;
  
  GLuint    fbo;
  GLuint    rbo;
  
  texture_t color_buffer;
} hdr_t;

bool hdr_init(hdr_t *hdr, mesh_t quad_mesh);
void hdr_begin(hdr_t *hdr);
void hdr_end(hdr_t *hdr);
void hdr_draw(hdr_t *hdr);

#endif
