#ifndef LIGHTING_H
#define LIGHTING_H

#define MAX_LIGHTS 32

#include "shader_attachment.hpp"
#include <util/math3d.hpp>
#include <opengl/uniform_buffer.hpp>
#include <opengl/shader.hpp>

class lighting_t : public shader_attachment_t {
private:
  uniform_buffer_t m_uniform_buffer;
  bool m_allocation[MAX_LIGHTS];

public:
  lighting_t();
  int add_light();
  int add_light(vec3 position, vec3 radiance);
  void set_light(int light, vec3 position, vec3 radiance);

  void attach_shader(const shader_t& shader);
};

#endif
