#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <vector>
#include "window.h"
#include "shader.h"
#include "vertex.h"
#include "target.h"
#include "camera.h"
#include "texture.h"
#include "vertex_buffer.h"
#include "uniform_buffer.h"

#define WIDTH 800
#define HEIGHT 600

int main() {
  window_t::init();
  window_t window(WIDTH, HEIGHT, "nui");
  
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  
  vertex_buffer_t vertex_buffer(256);
  
  mesh_t mesh = vertex_buffer.push(std::vector<vertex_t> {
    vertex_t(vec3(+1, +1, 5), vec2(1, 1)),
    vertex_t(vec3(-1, +1, 5), vec2(0, 1)),
    vertex_t(vec3(-1, -1, 5), vec2(0, 0)),
    vertex_t(vec3(+1, -1, 5), vec2(1, 0)),
    vertex_t(vec3(+1, +1, 5), vec2(1, 1)),
    vertex_t(vec3(-1, -1, 5), vec2(0, 0))
  });
  
  camera_t camera;
  
  std::stringstream src_vertex, src_fragment;
  camera.import_shader(src_vertex, src_fragment);
  src_vertex << std::ifstream("assets/shader.vert").rdbuf();
  src_fragment << std::ifstream("assets/shader.frag").rdbuf();
  shader_t shader(src_vertex, src_fragment);
  camera.attach_shader(shader);
  
  vertex_buffer.bind();
  
  float t = 0.0;
  
  while (window.poll()) {
    glClear(GL_COLOR_BUFFER_BIT);
    
    t += 0.015;
    
    shader.bind();
    camera.move(vec3(0, 0, cos(t * 2.0) * 4.0), vec3(0, cos(t), 0));
    camera.sub(mat4::identity());
    mesh.draw();
    
    window.swap();
  }
  
  return 0;
}
