#ifndef VERTEX_H
#define VERTEX_H

#include <util/math3d.hpp>

class vertex_t {
public:
  vec3 pos;
  vec2 uv;
  
  inline vertex_t(vec3 pos_, vec2 uv_)
    : pos(pos_), uv(uv_) {}
};

#endif
