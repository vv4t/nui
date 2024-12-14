#include "shader_builder.hpp"

shader_builder_t::shader_builder_t() {
  
}

shader_builder_t& shader_builder_t::source_vertex_shader(const char* path) {
  m_src_vertex << shader_read_source(path).rdbuf();
  return *this;
}

shader_builder_t& shader_builder_t::source_fragment_shader(const char* path) {
  m_src_fragment << shader_read_source(path).rdbuf();
  return *this;
}

shader_builder_t& shader_builder_t::attach(shader_attachment_t& shader_attachment) {
  m_attachments.push_back(shader_attachment);
  return *this;
}

shader_t shader_builder_t::create_frame_shader(const char* path) {
  return source_vertex_shader("assets/screen-space.vert").source_fragment_shader(path).compile();
}

shader_t shader_builder_t::compile() {
  shader_t shader = shader_t(m_src_vertex, m_src_fragment);
  
  for (shader_attachment_t& shader_attachment : m_attachments) {
    shader_attachment.attach_shader(shader);
  }

  return shader;
}
