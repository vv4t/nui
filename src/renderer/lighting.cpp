#include "lighting.hpp"

struct ubo_light {
  vec4 position;
  vec4 radiance;
};

struct ubo_lighting {
  ubo_light lights[MAX_LIGHTS];
};

lighting_t::lighting_t() : m_uniform_buffer(1, "ubo_lighting", sizeof(ubo_lighting)) {}

int lighting_t::add_light() {
  for (int i = 0; i < MAX_LIGHTS; i++) {
    if (!m_allocation[i]) {
      m_allocation[i] = true;
      return i;
    }
  }
  
  throw std::runtime_error("ran out of lights");
}

int lighting_t::add_light(vec3 position, vec3 radiance) {
  int light = add_light();
  set_light(light, position, radiance);
  return light;
}

void lighting_t::set_light(int light, vec3 position, vec3 radiance) {
  ubo_light data;
  data.position = vec4(position, 1.0);
  data.radiance = vec4(radiance, 1.0);
  m_uniform_buffer.sub(&data, sizeof(ubo_light) * light, sizeof(data));
}

void lighting_t::attach_shader(const shader_t& shader) {
  m_uniform_buffer.attach_shader(shader);
}
