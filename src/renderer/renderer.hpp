#ifndef RENDERER_H
#define RENDERER_H

#include "camera.hpp"
#include "material.hpp"
#include "lighting.hpp"
#include <core/game.hpp>
#include <opengl/texture.hpp>
#include <opengl/vertex_buffer.hpp>
#include <opengl/shader.hpp>
#include <opengl/target.hpp>
#include <vector>

class renderer_t {
private:
  vertex_buffer_t m_vertex_buffer;
  
  lighting_t m_lighting;
  camera_t m_camera;
  game_t& m_game;
  
  texture_t m_depth;
  texture_t m_normal;
  texture_t m_radiance;
  texture_t m_buffer[2];

  target_t m_target[2];
  target_t m_gbuffer_target;

  shader_t m_gbuffer;
  shader_t m_surface;
  shader_t m_deferred;
  shader_t m_dither;
  shader_t m_tone_map;
  
  std::vector<mesh_t> m_meshes;
  std::vector<texture_t> m_textures;
  std::vector<material_t> m_materials;
  
  void init_assets();
  
  void draw_entities();
  void draw_buffer(int width, int height, shader_t& shader);

public:
  renderer_t(game_t& game);
  void bind();
  void render();
};

#endif
