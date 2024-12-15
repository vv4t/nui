#include "mesh_builder.hpp"
#include <iostream>

void mesh_builder_t::push_vertex(vertex_t vertex) {
  m_vertices.push_back(vertex);
}

void mesh_builder_t::push_quad(mat4 T_p, mat4 T_uv) {
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
  
  vec3 v_n = (T_p * vec4(0, 0, 1, 1) - T_p * vec4()).get_xyz().normalize();
  
  for (int i = 0; i < 6; i++) {
    vec3 v_p = (T_p * vec4(quad_p[i], 1)).get_xyz();
    vec2 v_uv = (T_uv * vec4(quad_uv[i], 0, 1)).get_xy();
    push_vertex(vertex_t(v_p, v_n, v_uv));
  }
}

void mesh_builder_t::push_cuboid(vec3 a, vec3 b) {
  vec3 h = vec3(1, 0, 0);
  vec3 v = vec3(0, 1, 0);
  vec3 f = vec3(0, 0, 1);
  
  mat4 F = mat4(+h, +v, +f);
  mat4 B = mat4(+h, +v, -f);
  mat4 R = mat4(+f, +v, +h);
  mat4 L = mat4(+f, +v, -h);
  mat4 U = mat4(+h, +f, +v);
  mat4 D = mat4(+h, +f, -v);
  
  mat4 p = mat4::translate(f);
  mat4 q = mat4::scale(b - a) * mat4::translate(b) * mat4::scale(vec3(0.5));
  
  this->push_quad(p * F * q, mat4::identity());
  this->push_quad(p * B * q, mat4::identity());
  this->push_quad(p * R * q, mat4::identity());
  this->push_quad(p * L * q, mat4::identity());
  this->push_quad(p * U * q, mat4::identity());
  this->push_quad(p * D * q, mat4::identity());
}

void mesh_builder_t::solve_tangents() {
  for (int i = 0; i < m_vertices.size(); i += 3) {
    vertex_t& v1 = m_vertices[i + 0];
    vertex_t& v2 = m_vertices[i + 1];
    vertex_t& v3 = m_vertices[i + 2];

    vec3 d_pos1 = v2.pos - v1.pos;
    vec3 d_pos2 = v3.pos - v1.pos;
    
    vec2 d_uv1 = v2.uv - v1.uv;
    vec2 d_uv2 = v3.uv - v1.uv;
    
    float f = 1.0f / (d_uv1.x * d_uv2.y - d_uv2.x * d_uv1.y);
    
    vec3 tangent = (d_pos1 * d_uv2.y - d_pos2 * d_uv1.y) * f;
    vec3 bitangent = (d_pos2 * d_uv1.x - d_pos1 * d_uv2.x) * f;
    
    v1.tangent = tangent;
    v2.tangent = tangent;
    v3.tangent = tangent;
    
    v1.bitangent = bitangent;
    v2.bitangent = bitangent;
    v3.bitangent = bitangent;
  }
}

std::vector<vertex_t> mesh_builder_t::compile() {
  solve_tangents();
  return m_vertices;
}
