#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H

#include <opengl/vertex.hpp>
#include <vector>

class mesh_builder_t {
private:
  std::vector<vertex_t> m_vertices;
  void solve_tangents();

public:
  void push_vertex(vertex_t vertex);
  void push_quad(mat4 T_p, mat4 T_uv);
  void push_cuboid(vec3 a, vec3 b);
  std::vector<vertex_t> compile();
};

#endif
