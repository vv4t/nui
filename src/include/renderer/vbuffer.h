#ifndef VBUFFER_H
#define VBUFFER_H

#include <vertex.h>
#include <renderer/meshdata.h>

typedef struct {
  int offset;
  int count;
} mesh_t;

void vbuffer_init(int max_vertices);
mesh_t vbuffer_add(meshdata_t md);
void vbuffer_bind();
void vbuffer_draw(mesh_t mesh);
void vbuffer_deinit();

#endif
