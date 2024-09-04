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
  window_t window(800, 600, "yuu");
  
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  
  vertex_buffer_t vertex_buffer(1024);
  
  texture_t texture("assets/thing.jpg");
  texture_t buffer(800, 600, GL_RGBA, GL_UNSIGNED_BYTE);
  
  target_t to_buffer(1, GL_COLOR_ATTACHMENT0, buffer);
  
  std::vector<vertex_t> vertices;
  vertices.push_back(vertex_t(vec3(+1, +1, 0), vec2(1, 1)));
  vertices.push_back(vertex_t(vec3(-1, +1, 0), vec2(0, 1)));
  vertices.push_back(vertex_t(vec3(-1, -1, 0), vec2(0, 0)));
  vertices.push_back(vertex_t(vec3(+1, -1, 0), vec2(1, 0)));
  vertices.push_back(vertex_t(vec3(+1, +1, 0), vec2(1, 1)));
  vertices.push_back(vertex_t(vec3(-1, -1, 0), vec2(0, 0)));
  mesh_t mesh = vertex_buffer.push(vertices);
  
  std::stringstream src_vertex, src_fragment;
  src_vertex << std::ifstream("assets/shader.vert").rdbuf();
  src_fragment << std::ifstream("assets/shader.frag").rdbuf();
  shader_t shader(src_vertex, src_fragment);
  
  vertex_buffer.bind();
  shader.bind();
  texture.bind(0);
  
  while (window.poll()) {
    glClear(GL_COLOR_BUFFER_BIT);
    
    to_buffer.bind();
    mesh.draw();
    to_buffer.unbind();
    buffer.bind(0);
    mesh.draw();
    
    window.swap();
  }
  
  return 0;
}
