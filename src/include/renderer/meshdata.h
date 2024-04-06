#ifndef MESHDATA_H
#define MESHDATA_H

#include <math3d.h>
#include <vertex.h>

typedef struct meshdata_s *meshdata_t;

meshdata_t meshdata_create();
void meshdata_add_vertex(meshdata_t md, vertex_t v);
void meshdata_add_quad(meshdata_t md, matrix T_p);
const vertex_t *meshdata_get_vertices();
int meshdata_get_size();
void meshdata_destroy(meshdata_t md);

#endif
