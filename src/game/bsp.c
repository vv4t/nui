#include "bsp.h"

void bsp_clip_hull_R(trace_t *trace, const bsp_t *bsp, int node_id, const hull_t *hull, clip_t clip);
vec3_t hull_furthest_in(const hull_t *hull, vec3_t direction);

void bsp_clip_hull(trace_t *trace, const bsp_t *bsp, const hull_t *hull)
{
  *trace = (trace_t) {0};
  return bsp_clip_hull_R(trace, bsp, 0, hull, (clip_t) { .depth = -1000.0  });
}

void bsp_clip_hull_R(trace_t *trace, const bsp_t *bsp, int node_id, const hull_t *hull, clip_t clip)
{
  if (node_id == -1) {
    return;
  }
  
  const bsp_node_t *node = &bsp->nodes[node_id];
  
  vec3_t furthest_in = hull_furthest_in(hull, node->plane.normal);
  vec3_t furthest_out = hull_furthest_in(hull, vec3_mulf(node->plane.normal, -1));
  
  float min_depth = plane_depth(node->plane, furthest_in);
  float max_depth = plane_depth(node->plane, furthest_out);
  
  if (max_depth > 0) {
    bsp_clip_hull_R(trace, bsp, node->ahead, hull, clip);
  }
  
  if (min_depth < 0) {
    if (min_depth > clip.depth && !node->bevel) {
      clip.normal = node->plane.normal;
      clip.depth = min_depth;
    }
    
    if (node->behind == -1) {
      trace->clips[trace->num_clips++] = clip;
    }
    
    bsp_clip_hull_R(trace, bsp, node->behind, hull, clip);
  }
}

vec3_t hull_furthest_in(const hull_t *hull, vec3_t direction)
{
  float furthest_depth = vec3_dot(vec3_add(hull->pos, hull->vertices[0]), direction);
  vec3_t furthest_vertex = vec3_add(hull->pos, hull->vertices[0]);
  
  for (int i = 1; i < hull->num_vertices; i++) {
    float depth = vec3_dot(vec3_add(hull->pos, hull->vertices[i]), direction);
    
    if (depth < furthest_depth) {
      furthest_depth = depth;
      furthest_vertex = vec3_add(hull->pos, hull->vertices[i]);
    }
  }
  
  return furthest_vertex;
}
