#ifndef MATERIAL_H
#define MATERIAL_H

#include <opengl/texture.hpp>

class material_t {
public:
  texture_t& albedo;

  material_t(texture_t& _albedo) : albedo(_albedo) {}
};

#endif
