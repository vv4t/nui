#include "hull.h"

vec3_t hull_furthest_in(const hull_t *hull, vec3_t d)
{
  float min_depth = vec3_dot(vec3_add(hull->pos, hull->vertices[0]), d);
  int min_vertex = 0;
  
  for (int i = 1; i < hull->num_vertices; i++) {
    float depth = vec3_dot(vec3_add(hull->pos, hull->vertices[i]), d);
    
    if (depth < min_depth) {
      min_depth = depth;
      min_vertex = i;
    }
  }
  
  return vec3_add(hull->pos, hull->vertices[min_vertex]);
}

plane_t hull_calc_plane(const hull_t *hull, int plane_num)
{
  plane_t plane = hull->planes[plane_num];
  plane.distance += vec3_dot(hull->pos, plane.normal);
  return plane;
}
