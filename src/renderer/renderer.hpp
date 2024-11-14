#ifndef RENDERER_H
#define RENDERER_H

#include "camera.hpp"
#include <core/game.hpp>
#include <opengl/vertex_buffer.hpp>
#include <opengl/shader.hpp>

class renderer_t {
private:
  vertex_buffer_t m_vertex_buffer;
  camera_t m_camera;
  shader_t m_shader;
  mesh_t m_mesh;
  game_t& m_game;
  
  shader_t shader_init();
  mesh_t mesh_init();

public:
  renderer_t(game_t& game);
  void bind();
  void render();
};

#endif
