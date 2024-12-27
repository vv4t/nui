#ifndef SHADER_H
#define SHADER_H

#include <sstream>
#include <vector>
#include <util/math3d.hpp>
#include <glad/glad.h>

class shader_t {
private:
  GLuint m_program;

public:
  shader_t(const std::stringstream& src_vertex, const std::stringstream& src_fragment);
  ~shader_t();
  void bind() const;
  shader_t& uniform_int(const char* name, int value);
  shader_t& uniform_float(const char* name, float value);
  shader_t& uniform_vec3(const char* name, vec3 value);
  shader_t& uniform_vec3_array(const char* name, std::vector<vec3> value);
  GLuint get_program() const;
};

std::stringstream shader_read_source(const char* src);

#endif
