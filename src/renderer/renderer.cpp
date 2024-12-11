#include "renderer.hpp"
#include "mesh_builder.hpp"
#include "shader_builder.hpp"
#include <iostream>

renderer_t::renderer_t(game_t& game)
  : m_vertex_buffer(256),
    m_game(game),
    m_texture("assets/bocchi.jpg"),
    m_buffer(800, 600, GL_RGBA, GL_RGBA32F, GL_FLOAT),
    m_depth(800, 600, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16, GL_FLOAT),
    m_target({ binding_t(GL_COLOR_ATTACHMENT0, m_buffer), binding_t(GL_DEPTH_ATTACHMENT, m_depth) }),
    m_surface(
      shader_builder_t()
      .source_vertex_shader("assets/shader.vert")
      .source_fragment_shader("assets/shader.frag")
      .compile()
    ),
    m_frame(
      shader_builder_t()
      .source_vertex_shader("assets/frame.vert")
      .source_fragment_shader("assets/frame.frag")
      .compile()
    )
{
  meshes_init();
}

void renderer_t::bind() {
  m_vertex_buffer.bind();
}

void renderer_t::render() {
  transform_t &camera_transform = m_game.get_transform(m_game.get_camera());
  
  m_camera.move(camera_transform.position, camera_transform.rotation);
  
  m_target.bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_surface.bind();
  m_texture.bind(0);
  draw_entities();
  m_target.unbind();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_frame.bind();
  m_buffer.bind(0);
  m_meshes[0].draw();
}

void renderer_t::draw_entities() {
  for (entity_t entity = 0; entity < m_game.entity_count(); entity++) {
    if (m_game.has_component(entity, HAS_MODEL | HAS_TRANSFORM)) {
      transform_t& transform = m_game.get_transform(entity);
      model_t& model = m_game.get_model(entity);
      
      mat4 T_rotation = mat4::rotate_zyx(transform.rotation);
      mat4 T_translation = mat4::translate(transform.position);
      mat4 T_scale = mat4::scale(transform.scale);
      
      m_camera.sub(T_rotation * T_scale * T_translation);
      m_meshes[model.meshname].draw();
    }
  }
}

void renderer_t::meshes_init() {
  mesh_builder_t mesh_builder;

  mesh_builder = mesh_builder_t();
  mesh_builder.push_quad(mat4::identity(), mat4::identity());
  m_meshes[MESHNAME_PLANE] = m_vertex_buffer.push(mesh_builder.get_vertices());
  
  mesh_builder = mesh_builder_t();
  mesh_builder.push_cuboid(vec3(0.0), vec3(1.0));
  m_meshes[MESHNAME_CUBOID] = m_vertex_buffer.push(mesh_builder.get_vertices());
}

