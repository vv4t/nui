#include "bsp.h"

#include <stdlib.h>
#include <string.h>

static void bsp_free(bsp_t *bsp);
static void bsp_clip_R(trace_t *trace, const bsp_t *bsp, const hull_t *hull, int node_num);
static void hull_clip_bsp_hull(trace_t *trace, const bsp_t *bsp, map_hull_t bsp_hull, const hull_t *hull);
plane_t hull_calc_plane(const hull_t *hull, int plane_num);
vec3_t hull_furthest_in(const hull_t *hull, vec3_t d);
vec3_t bsp_hull_furthest_in(const bsp_t *bsp, map_hull_t hull, vec3_t d);

void bsp_load(bsp_t *bsp, const map_t *map)
{
  bsp_free(bsp);
  
  bsp->num_vertices = map->num_vertices;
  bsp->num_planes = map->num_planes;
  bsp->num_hulls = map->num_hulls;
  bsp->num_nodes = map->num_nodes;
  
  bsp->vertices = calloc(map->num_vertices, sizeof(map_vertex_t));
  bsp->planes = calloc(map->num_planes, sizeof(plane_t));
  bsp->hulls = calloc(map->num_hulls, sizeof(map_hull_t));
  bsp->nodes = calloc(map->num_nodes, sizeof(map_bsp_node_t));
  
  memcpy(bsp->vertices, map->vertices, map->num_vertices * sizeof(map_vertex_t));
  memcpy(bsp->planes, map->planes, map->num_planes * sizeof(plane_t));
  memcpy(bsp->hulls, map->hulls, map->num_hulls * sizeof(map_hull_t));
  memcpy(bsp->nodes, map->nodes, map->num_nodes* sizeof(map_bsp_node_t));
}

void bsp_clip(trace_t *trace, const bsp_t *bsp, const hull_t *hull)
{
  trace->num_clips = 0;
  bsp_clip_R(trace, bsp, hull, 0);
}

static void bsp_clip_R(trace_t *trace, const bsp_t *bsp, const hull_t *hull, int node_num)
{
  if (node_num == -1) {
    return;
  }
  
  map_bsp_node_t node = bsp->nodes[node_num];
  plane_t plane = bsp->planes[node.plane];
  
  vec3_t min_v = hull_furthest_in(hull, plane.normal);
  vec3_t max_v = hull_furthest_in(hull, vec3_mulf(plane.normal, -1));
  
  float min_depth = plane_depth(plane, min_v);
  float max_depth = plane_depth(plane, max_v);
  
  if (max_depth > 0) {
    bsp_clip_R(trace, bsp, hull, node.ahead);
  }
  
  if (min_depth < 0) {
    if (node.behind == -1) {
      hull_clip_bsp_hull(trace, bsp, bsp->hulls[node.hull], hull);
    }
    
    bsp_clip_R(trace, bsp, hull, node.behind);
  }
}

static void hull_clip_bsp_hull(trace_t *trace, const bsp_t *bsp, map_hull_t bsp_hull, const hull_t *hull)
{
  float min_depth = -10000.0;
  plane_t min_plane;
  
  for (int i = bsp_hull.plane_offset; i < bsp_hull.plane_offset + bsp_hull.plane_count; i++) {
    plane_t plane = bsp->planes[i];
    
    vec3_t vertex = hull_furthest_in(hull, plane.normal);
    float depth = plane_depth(plane, vertex);
    
    if (depth > 0) {
      return;
    } else if (depth > min_depth) {
      min_depth = depth;
      min_plane = plane;
    }
  }
  
  for (int i = 0; i < hull->num_planes; i++) {
    plane_t plane = hull_calc_plane(hull, i);
    
    vec3_t vertex = bsp_hull_furthest_in(bsp, bsp_hull, plane.normal);
    float depth = plane_depth(plane, vertex);
    
    if (depth > 0) {
      return;
    } else if (depth > min_depth) {
      min_depth = depth;
      min_plane = plane;
    }
  }
  
  int clip_num = trace->num_clips++;
  trace->clips[clip_num].plane = min_plane;
  trace->clips[clip_num].depth = min_depth;
}

static void bsp_free(bsp_t *bsp)
{
  if (bsp->vertices) {
    free(bsp->vertices);
  }
  
  if (bsp->planes) {
    free(bsp->vertices);
  }
  
  if (bsp->hulls) {
    free(bsp->planes);
  }
  
  if (bsp->nodes) {
    free(bsp->nodes);
  }
  
  bsp->vertices = NULL;
  bsp->planes = NULL;
  bsp->hulls = NULL;
  bsp->nodes = NULL;
}

vec3_t bsp_hull_furthest_in(const bsp_t *bsp, map_hull_t hull, vec3_t d)
{
  float min_depth = vec3_dot(bsp->vertices[hull.vertex_offset].pos, d);
  int min_vertex = hull.vertex_offset;
  
  for (int i = hull.vertex_offset + 1; i < hull.vertex_offset + hull.vertex_count; i++) {
    float depth = vec3_dot(bsp->vertices[i].pos, d);
    
    if (depth < min_depth) {
      min_depth = depth;
      min_vertex = i;
    }
  }
  
  return bsp->vertices[min_vertex].pos;
}
