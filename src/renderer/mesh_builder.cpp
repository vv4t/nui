#include "mesh_builder.hpp"
#include <iostream>

void mesh_builder_t::add_vertex(vertex_t vertex) {
  m_vertices.push_back(vertex);
}

void mesh_builder_t::add_quad(mat4 T_p, mat4 T_uv) {
  vec3 quad_p[] = {
    vec3(+1, +1, 0),
    vec3(-1, +1, 0),
    vec3(-1, -1, 0),
    vec3(+1, -1, 0),
    vec3(+1, +1, 0),
    vec3(-1, -1, 0)
  };
  
  vec2 quad_uv[] = {
    vec2(1, 1),
    vec2(0, 1),
    vec2(0, 0),
    vec2(1, 0),
    vec2(1, 1),
    vec2(0, 0)
  };
  
  for (int i = 0; i < 6; i++) {
    vec3 v_p = (T_p * vec4(quad_p[i], 1)).get_xyz();
    vec2 v_uv = (T_p * vec4(quad_uv[i], 0, 1)).get_xy();
    add_vertex(vertex_t(v_p, v_uv));
  }
}

std::vector<vertex_t> mesh_builder_t::get_vertices() {
  return m_vertices;
}
