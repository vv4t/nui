#ifndef GLS_FLAT_H
#define GLS_FLAT_H

#include "gl.h"

typedef struct {
  GLuint program;
} gls_flat_t;

bool gls_flat_init(gls_flat_t *gls_flat);
void gls_flat_bind(gls_flat_t *gls_flat);

#endif
