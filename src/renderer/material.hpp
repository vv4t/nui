#ifndef MATERIAL_H
#define MATERIAL_H

#include <opengl/texture.hpp>

class material_t {
public:
  texture_t& albedo;
  texture_t& normal;

  material_t(texture_t& _albedo, texture_t& _normal)
    : albedo(_albedo),
    normal(_normal)
    {}
};

#endif
