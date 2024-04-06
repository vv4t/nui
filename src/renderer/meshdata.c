#include <renderer/meshdata.h>

#include <stdio.h>
#include <stdlib.h>

struct meshdata_s {
  vertex_t *vertices;
  int pos;
  int size;
};

meshdata_t meshdata_create()
{
  meshdata_t md = calloc(sizeof(*md), 1);
  md->pos = 0;
  md->size = 32;
  md->vertices = calloc(sizeof(*md->vertices), md->size);
  return md;
}

void meshdata_add_quad(meshdata_t md, matrix T_p)
{
  vector quad[] = {
    vec2(-1, -1),
    vec2(-1, +1),
    vec2(+1, -1),
    vec2(-1, +1),
    vec2(+1, +1),
    vec2(+1, -1)
  };
  
  for (int i = 0; i < sizeof(quad) / sizeof(quad[0]); i++) {
    vector p = mdotv(T_p, v2pt(quad[i]));
    meshdata_add_vertex(md, vertex_create(p));
  }
}

void meshdata_add_vertex(meshdata_t md, vertex_t v)
{
  if (md->pos >= md->size) {
    md->size *= 2;
    md->vertices = realloc(md->vertices, md->size);
  }
  
  md->vertices[md->pos++] = v;
}

const vertex_t *meshdata_get_vertices(meshdata_t md)
{
  return md->vertices;
}

int meshdata_get_size(meshdata_t md)
{
  return md->pos;
}

void meshdata_destroy(meshdata_t md)
{
  free(md->vertices);
  free(md);
}

