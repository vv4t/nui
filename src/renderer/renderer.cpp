#include "renderer.hpp"
#include "mesh_builder.hpp"
#include <iostream>

renderer_t::renderer_t(game_t& game)
  : m_vertex_buffer(256),
    m_shader(shader_init()),
    m_texture("assets/bocchi.jpg"),
    m_mesh(mesh_init()),
    m_game(game)
{
}

void renderer_t::bind() {
  m_vertex_buffer.bind();
}

void renderer_t::render() {
  m_texture.bind(0);
  
  transform_t &camera_transform = m_game.get_transform(m_game.get_camera());
  
  m_camera.move(camera_transform.position, camera_transform.rotation);
  m_shader.bind();
  
  for (entity_t entity = 0; entity < m_game.entity_count(); entity++) {
    if (m_game.has_component(entity, HAS_MODEL | HAS_TRANSFORM)) {
      transform_t& transform = m_game.get_transform(entity);
      model_t& model = m_game.get_model(entity);
      
      if (model.meshname == MESHNAME_PLANE) {
        m_camera.sub(mat4::rotate_zyx(transform.rotation) * mat4::translate(transform.position));
        m_mesh.draw();
      }
    }
  }
}

mesh_t renderer_t::mesh_init() {
  mesh_builder_t mesh_builder;
  mesh_builder.push_cuboid(vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
  return m_vertex_buffer.push(mesh_builder.get_vertices());
}

shader_t renderer_t::shader_init() {
  std::stringstream src_vertex, src_fragment;
  src_vertex << shader_read_source("assets/shader.vert").rdbuf();
  src_fragment << shader_read_source("assets/shader.frag").rdbuf();
  shader_t shader = shader_t(src_vertex, src_fragment);
  m_camera.attach_shader(shader);
  return shader;
}
