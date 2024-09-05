#ifndef CAMERA_H
#define CAMERA_H

#include "math3d.h"
#include "uniform_buffer.h"
#include "shader.h"

class camera_t {
private:
  mat4 m_project;
  mat4 m_view;
  uniform_buffer_t m_uniform_buffer;

public:
  camera_t();
  void import_shader(std::ostream& src_vertex, std::ostream& src_fragment);
  void attach_shader(const shader_t& shader);
  void move(vec3 position, vec3 rotation);
  void sub(mat4 model);
};

#endif
