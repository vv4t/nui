#ifndef CAMERA_H
#define CAMERA_H

#include "shader_attachment.hpp"
#include <util/math3d.hpp>
#include <opengl/uniform_buffer.hpp>
#include <opengl/shader.hpp>

class camera_t : public shader_attachment_t {
private:
  mat4 m_project;
  mat4 m_view;
  vec3 m_view_pos;
  uniform_buffer_t m_uniform_buffer;

public:
  camera_t();
  void move(vec3 position, vec3 rotation);
  void sub(mat4 model);
  
  void attach_shader(const shader_t& shader) override;
};

#endif
