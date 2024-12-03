#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H

#include <opengl/vertex.hpp>
#include <vector>

class mesh_builder_t {
  std::vector<vertex_t> m_vertices;

public:
  void add_vertex(vertex_t vertex);
  void add_quad(mat4 T_p, mat4 T_uv);
  std::vector<vertex_t> get_vertices();
};

#endif
