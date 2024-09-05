#include "camera.h"

struct ubo_camera {
  mat4 MVP;
};

camera_t::camera_t() : m_uniform_buffer(0, "ubo_camera", 512) {
  m_project = mat4::perspective();
  m_view = mat4::identity();
}

void camera_t::sub(mat4 model) {
  struct ubo_camera data;
  data.MVP = model * m_view * m_project;
  m_uniform_buffer.sub(&data, 0, sizeof(data));
}

void camera_t::move(vec3 position, vec3 rotation) {
  mat4 rx = mat4::rotate_x(rotation.x);
  mat4 ry = mat4::rotate_y(rotation.y);
  mat4 rz = mat4::rotate_z(rotation.z);
  mat4 translate = mat4::translate(-position);
  
  m_view = translate * rz * ry * rx;
}

void camera_t::import_shader(std::ostream& src_vertex, std::ostream& src_fragment) {
  const char* text = "layout(std140) uniform ubo_camera { mat4 MVP; };";
  src_vertex << text << std::endl;
  src_fragment << text << std::endl;
}

void camera_t::attach_shader(const shader_t& shader) {
  m_uniform_buffer.attach_shader(shader);
}
