#ifndef SHADER_BUILDER_H
#define SHADER_BUILDER_H

#include <opengl/shader.hpp>
#include <iostream>

class shader_builder_t {
private:
  std::stringstream m_src_vertex;
  std::stringstream m_src_fragment;

public:
  shader_builder_t();
  shader_builder_t& source_vertex_shader(const char* path);
  shader_builder_t& source_fragment_shader(const char* path);
  shader_t compile();
};

#endif
