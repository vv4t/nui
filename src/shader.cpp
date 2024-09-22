#include "shader.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <filesystem>

static GLuint shader_compile(GLuint type, const char* src);

shader_t::shader_t(const std::stringstream& src_vertex, const std::stringstream& src_fragment) {
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

void shader_t::bind() const {
  glUseProgram(m_program);
}

GLuint shader_t::get_program() const {
  return m_program;
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

std::stringstream shader_read_source(const char* src) {
  std::stringstream ss;
  std::ifstream in = std::ifstream(src);
  
  std::string line;
  while (std::getline(in, line)) {
    std::regex rgx("^\\s*#\\s*pragma use\\s+[<\"]([^>\"]*)[>\"]\\s*");
    std::smatch matches;

    if(std::regex_match(line, matches, rgx)) {
      std::filesystem::path file(matches[1]);
      std::filesystem::path base = std::filesystem::path(src).parent_path();
      std::filesystem::path full = base / file;
      ss << shader_read_source(full.u8string().c_str()).rdbuf() << std::endl;
    } else {
      ss << line << std::endl;
    }
  }
  
  return ss;
}
