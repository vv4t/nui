#ifndef MATERIAL_H
#define MATERIAL_H

#include <opengl/texture.hpp>

class material_t {
public:
  texture_t& albedo;
  texture_t& normal;
  texture_t& roughness;

  material_t(texture_t& _albedo, texture_t& _normal, texture_t& _roughness)
    : albedo(_albedo),
      normal(_normal),
      roughness(_roughness)
    {}
};

#endif
