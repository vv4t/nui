#ifndef VERTEX_H
#define VERTEX_H

#include "math3d.h"

class vertex_t {
public:
  vec3 pos;
  vec2 uv;
  
  inline vertex_t(vec3 _pos, vec2 _uv) {
    pos = _pos;
    uv = _uv;
  }
};

#endif
