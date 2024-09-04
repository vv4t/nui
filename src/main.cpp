#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <vector>
#include "window.h"
#include "shader.h"
#include "vertex.h"
#include "vertex_buffer.h"
#include "texture.h"
#include "target.h"

int main() {
  window_t::init();
  window_t window(800, 600, "nui");
  
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  
  vertex_buffer_t vertex_buffer(256);
  
  texture_t buffer1(800, 600, GL_RGBA, GL_RGBA, GL_UNSIGNED_INT);
  texture_t buffer2(800, 600, GL_RGBA, GL_RGBA, GL_UNSIGNED_INT);
  
  target_t target(std::vector<binding_t> {
    binding_t(GL_COLOR_ATTACHMENT0, buffer1),
    binding_t(GL_COLOR_ATTACHMENT1, buffer2)
  });
  
  mesh_t mesh = vertex_buffer.push(std::vector<vertex_t> {
    vertex_t(vec3(+1, +1, 0), vec2(1, 1)),
    vertex_t(vec3(-1, +1, 0), vec2(0, 1)),
    vertex_t(vec3(-1, -1, 0), vec2(0, 0)),
    vertex_t(vec3(+1, -1, 0), vec2(1, 0)),
    vertex_t(vec3(+1, +1, 0), vec2(1, 1)),
    vertex_t(vec3(-1, -1, 0), vec2(0, 0))
  });
  
  std::stringstream src_vertex, src_fragment;
  src_vertex << std::ifstream("assets/shader.vert").rdbuf();
  src_fragment << std::ifstream("assets/shader.frag").rdbuf();
  shader_t shader(src_vertex, src_fragment);
  
  src_vertex = std::stringstream(), src_fragment = std::stringstream();
  src_vertex << std::ifstream("assets/shader.vert").rdbuf();
  src_fragment << std::ifstream("assets/test.frag").rdbuf();
  shader_t test(src_vertex, src_fragment);
  
  vertex_buffer.bind();
  
  while (window.poll()) {
    glClear(GL_COLOR_BUFFER_BIT);
    
    target.bind();
    test.bind();
    mesh.draw();
    target.unbind();
    
    shader.bind();
    buffer1.bind(1);
    mesh.draw();
    
    window.swap();
  }
  
  return 0;
}
