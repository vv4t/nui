#ifndef RENDERER_H
#define RENDERER_H

#include "camera.hpp"
#include "material.hpp"
#include <core/game.hpp>
#include <opengl/texture.hpp>
#include <opengl/vertex_buffer.hpp>
#include <opengl/shader.hpp>
#include <opengl/target.hpp>
#include <vector>

class renderer_t {
private:
  vertex_buffer_t m_vertex_buffer;
  
  camera_t m_camera;
  game_t& m_game;
  
  texture_t m_buffer;
  texture_t m_depth;
  target_t m_target;
  shader_t m_surface;
  shader_t m_frame;
  
  std::vector<mesh_t> m_meshes;
  std::vector<texture_t> m_textures;
  std::vector<material_t> m_materials;
  
  void assets_init();

public:
  renderer_t(game_t& game);
  void bind();
  void render();
  void draw_entities();
};

#endif
