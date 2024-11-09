#ifndef RENDERER_H
#define RENDERER_H

#include "camera.hpp"
#include <opengl/vertex_buffer.hpp>
#include <opengl/shader.hpp>
#include <core/input.hpp>

class renderer_t {
private:
  vertex_buffer_t m_vertex_buffer;
  camera_t m_camera;
  shader_t m_shader;
  mesh_t m_mesh;
  
  shader_t shader_init();
  mesh_t mesh_init();

public:
  renderer_t();
  void bind();
  void render(input_t& input);
};

#endif
