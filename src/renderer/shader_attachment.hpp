#ifndef SHADER_ATTACHMENT_HPP
#define SHADER_ATTACHMENT_HPP

#include <opengl/shader.hpp>
#include <functional>

class shader_attachment_t {
public:
  shader_attachment_t() {}
  virtual void attach_shader(const shader_t& shader) = 0;
};

using shader_attachment_ref_t = std::reference_wrapper<shader_attachment_t>;

#endif
