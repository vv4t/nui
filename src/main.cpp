#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <vector>
#include "window.h"
#include "shader.h"
#include "vertex.h"
#include "texture.h"
#include "target.h"
#include "vertex_buffer.h"
#include "uniform_buffer.h"

int main() {
  window_t::init();
  window_t window(800, 600, "nui");
  
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  
  vertex_buffer_t vertex_buffer(256);
  uniform_buffer_t uniform_buffer(0, "ubo_camera", 32);
  
  mesh_t mesh = vertex_buffer.push(std::vector<vertex_t> {
    vertex_t(vec3(+1, +1, 0), vec2(1, 1)),
    vertex_t(vec3(-1, +1, 0), vec2(0, 1)),
    vertex_t(vec3(-1, -1, 0), vec2(0, 0)),
    vertex_t(vec3(+1, -1, 0), vec2(1, 0)),
    vertex_t(vec3(+1, +1, 0), vec2(1, 1)),
    vertex_t(vec3(-1, -1, 0), vec2(0, 0))
  });
  
  vec3 thing = vec3(0.5, 0.0, 0.0);
  uniform_buffer.sub(&thing, 0, sizeof(thing));
  
  std::stringstream src_vertex, src_fragment;
  src_vertex << std::ifstream("assets/shader.vert").rdbuf();
  src_fragment << std::ifstream("assets/shader.frag").rdbuf();
  shader_t shader(src_vertex, src_fragment);
  
  vertex_buffer.bind();
  
  while (window.poll()) {
    glClear(GL_COLOR_BUFFER_BIT);
    
    shader.bind();
    mesh.draw();
    
    window.swap();
  }
  
  return 0;
}
