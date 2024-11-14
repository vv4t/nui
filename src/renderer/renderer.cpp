#include "renderer.hpp"
#include <iostream>

renderer_t::renderer_t(game_t& game)
  : m_vertex_buffer(256),
    m_shader(shader_init()),
    m_mesh(mesh_init()),
    m_game(game) {
}

void renderer_t::bind() {
  m_vertex_buffer.bind();
}

void renderer_t::render(input_t& input) {
  m_camera.move(vec3(0, 0, -3), vec3(input.get_axis(1), input.get_axis(0), 0.0));
  m_shader.bind();
  
  for (entity_t entity = 0; entity < m_game.count_entities(); entity++) {
    if (m_game.has_component(entity, HAS_MODEL | HAS_TRANSFORM)) {
      transform_t& transform = m_game.get_transform(entity);
      model_t& model = m_game.get_model(entity);
      
      m_camera.sub(mat4::rotate_zyx(transform.rotation) * mat4::translate(transform.position));
      m_mesh.draw();
    }
  }
}

mesh_t renderer_t::mesh_init() {
  return m_vertex_buffer.push(
    std::vector<vertex_t> {
      vertex_t(vec3(+1, +1, 0), vec2(1, 1)),
      vertex_t(vec3(-1, +1, 0), vec2(0, 1)),
      vertex_t(vec3(-1, -1, 0), vec2(0, 0)),
      vertex_t(vec3(+1, -1, 0), vec2(1, 0)),
      vertex_t(vec3(+1, +1, 0), vec2(1, 1)),
      vertex_t(vec3(-1, -1, 0), vec2(0, 0))
    }
  );
}

shader_t renderer_t::shader_init() {
  std::stringstream src_vertex, src_fragment;
  src_vertex << shader_read_source("assets/shader.vert").rdbuf();
  src_fragment << shader_read_source("assets/shader.frag").rdbuf();
  shader_t shader = shader_t(src_vertex, src_fragment);
  m_camera.attach_shader(shader);
  return shader;
}
