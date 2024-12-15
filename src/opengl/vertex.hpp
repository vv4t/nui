#ifndef VERTEX_H
#define VERTEX_H

#include <util/math3d.hpp>

class vertex_t {
public:
  vec3 pos;
  vec3 normal;
  vec3 tangent;
  vec3 bitangent;
  vec2 uv;
  
  inline vertex_t(vec3 pos_, vec3 _normal, vec2 uv_)
    : pos(pos_),
      normal(_normal),
      uv(uv_)
    {}
  
  inline vertex_t(vec3 pos_, vec3 _normal, vec3 _tangent, vec3 _bitangent, vec2 uv_)
    : pos(pos_),
      normal(_normal),
      tangent(_tangent),
      bitangent(_tangent),
      uv(uv_)
    {}
};

#endif
