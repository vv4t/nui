#ifndef VERTEX_H
#define VERTEX_H

#include <math3d.h>

typedef struct {
  float p[3];
  float n[3];
  float t[3];
  float bt[3];
  float uv[2];
} vertex_t;

inline static vertex_t vertex_create(vector p)
{
  return (vertex_t) {
    .p = { p.x, p.y, p.z }
  };
}

#endif
