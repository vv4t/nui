#include "renderer.hpp"
#include "mesh_builder.hpp"
#include <iostream>

renderer_t::renderer_t(game_t& game)
  : m_vertex_buffer(256),
    m_shader(shader_init()),
    m_texture("assets/bocchi.jpg"),
    m_game(game)
{
  meshes_init();
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
      
      mat4 T_rotation = mat4::rotate_zyx(transform.rotation);
      mat4 T_translation = mat4::translate(transform.position);
      mat4 T_scale = mat4::scale(transform.scale);
      
      m_camera.sub(T_rotation * T_translation * T_scale);
      m_meshes[model.meshname].draw();
    }
  }
}

void renderer_t::meshes_init() {
  mesh_builder_t mesh_builder;
  mesh_builder.push_quad(mat4::translate(vec3(1, 1, 0)) * mat4(vec3(1, 0, 0), vec3(0, 0, 1), vec3(0, 1, 0)), mat4::identity());
  m_meshes[MESHNAME_PLANE] = m_vertex_buffer.push(mesh_builder.get_vertices());
  
  mesh_builder = mesh_builder_t();
  mesh_builder.push_cuboid(vec3(0.0), vec3(1.0));
  m_meshes[MESHNAME_CUBOID] = m_vertex_buffer.push(mesh_builder.get_vertices());
}

shader_t renderer_t::shader_init() {
  std::stringstream src_vertex, src_fragment;
  src_vertex << shader_read_source("assets/shader.vert").rdbuf();
  src_fragment << shader_read_source("assets/shader.frag").rdbuf();
  shader_t shader = shader_t(src_vertex, src_fragment);
  m_camera.attach_shader(shader);
  return shader;
}
