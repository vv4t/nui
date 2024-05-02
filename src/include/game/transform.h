#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <math3d.h>

typedef struct {
  vector position;
  vector rotation;
  vector scale;
} transform_t;

inline static transform_t create_transform()
{
  return (transform_t) {
    .position = vec3(0, 0, 0),
    .rotation = vec3(0, 0, 0),
    .scale = vec3(1, 1, 1)
  };
}

#endif
