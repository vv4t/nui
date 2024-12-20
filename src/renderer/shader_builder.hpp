#ifndef SHADER_BUILDER_H
#define SHADER_BUILDER_H

#include "shader_attachment.hpp"
#include <opengl/shader.hpp>
#include <iostream>

class shader_builder_t {
private:
  std::stringstream m_src_vertex;
  std::stringstream m_src_fragment;
  std::vector<shader_attachment_ref_t> m_attachments;
  std::vector<std::pair<const char*, int>> m_bindings;

public:
  shader_builder_t();
  shader_builder_t& source_vertex_shader(const char* path);
  shader_builder_t& source_fragment_shader(const char* path);
  shader_builder_t& attach(shader_attachment_t& shader_attachment);
  shader_builder_t& bind(const char* name, int channel);
  shader_t create_frame_shader(const char* path);
  shader_t compile();
};

#endif
