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

shader_t shader_builder_t::compile() {
  return shader_t(m_src_vertex, m_src_fragment);
}
