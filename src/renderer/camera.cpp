#include "camera.hpp"
#include <iostream>

struct ubo_camera {
  mat4 MVP;
};

camera_t::camera_t() : m_uniform_buffer(0, "ubo_camera", 512) {
  m_project = mat4::perspective(600.0 / 800.0, M_PI / 2.0, 0.1, 100.0);
  m_view = mat4::identity();
}

void camera_t::sub(mat4 model) {
  struct ubo_camera data;
  data.MVP = model * m_view * m_project;
  m_uniform_buffer.sub(&data, 0, sizeof(data));
}

void camera_t::move(vec3 position, vec3 rotation) {
  mat4 rx = mat4::rotate_x(-rotation.x);
  mat4 ry = mat4::rotate_y(-rotation.y);
  mat4 rz = mat4::rotate_z(-rotation.z);
  mat4 translate = mat4::translate(-position);
  
  m_view = translate * rz * ry * rx;
}

void camera_t::attach_shader(const shader_t& shader) {
  m_uniform_buffer.attach_shader(shader);
}
