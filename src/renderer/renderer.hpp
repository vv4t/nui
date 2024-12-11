#ifndef RENDERER_H
#define RENDERER_H

#include "camera.hpp"
#include <core/game.hpp>
#include <opengl/texture.hpp>
#include <opengl/vertex_buffer.hpp>
#include <opengl/shader.hpp>
#include <opengl/target.hpp>

class renderer_t {
private:
  vertex_buffer_t m_vertex_buffer;
  
  camera_t m_camera;
  game_t& m_game;
  
  texture_t m_texture;
  texture_t m_buffer;
  texture_t m_depth;
  target_t m_target;
  shader_t m_surface;
  shader_t m_frame;
  
  mesh_t m_meshes[MAX_MESHNAME];
  
  void meshes_init();

public:
  renderer_t(game_t& game);
  void bind();
  void render();
  void draw_entities();
};

#endif
