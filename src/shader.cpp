#include "shader.h"
#include <iostream>

static GLuint shader_compile(GLuint type, const char* src);

shader_t::shader_t(std::stringstream& src_vertex, std::stringstream& src_fragment) {
  GLuint vertex_shader = shader_compile(GL_VERTEX_SHADER, src_vertex.str().c_str());
  GLuint fragment_shader = shader_compile(GL_FRAGMENT_SHADER, src_fragment.str().c_str());
  
  m_program = glCreateProgram();
  
  glAttachShader(m_program, vertex_shader);
  glAttachShader(m_program, fragment_shader);
  
  int success;
  glLinkProgram(m_program);
  glGetProgramiv(m_program, GL_LINK_STATUS, &success);
  
  if (!success) {
    static char info[1024];
    glGetProgramInfoLog(m_program, sizeof(info), NULL, info);
    std::cerr << "error: shader_compile: " << info;
    throw std::runtime_error("failed to link shader");
  }
  
  glDetachShader(m_program, vertex_shader);
  glDetachShader(m_program, vertex_shader);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}

void shader_t::bind() {
  glUseProgram(m_program);
}

void shader_t::attach(const uniform_buffer_t &uniform_buffer) {
  bind();
  GLuint location = glGetUniformBlockIndex(m_program, uniform_buffer.get_name());
  glUniformBlockBinding(m_program, location, uniform_buffer.get_binding());
}

shader_t::~shader_t() {
  glDeleteProgram(m_program);
}

GLuint shader_compile(GLuint type, const char* src) {
  const char *all[] = {
    "#version 300 es\n",
    "precision mediump float;\n",
    src
  };
  
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 3, all, NULL);
  
  int success;
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  
  if (!success) {
    static GLchar info[1024];
    glGetShaderInfoLog(shader, sizeof(info), NULL, info);
    std::cerr << "error: shader_compile: " << info;
    throw std::runtime_error("failed to compile shader");
  }
  
  return shader;
}
