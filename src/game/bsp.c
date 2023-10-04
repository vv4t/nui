#include "bsp.h"

int bsp_clip_hull_R(
  clip_t        clips[],
  const bsp_t   *bsp,
  int           node_id,
  const hull_t  *hull,
  bsp_plane_t   min_plane,
  float         min_plane_dist,
  int           num_clips);

int bsp_clip_hull(clip_t clips[], const bsp_t *bsp, const hull_t *hull)
{
  return bsp_clip_hull_R(clips, bsp, 0, hull, (bsp_plane_t) {0}, -1000, 0);
}

int bsp_clip_hull_R(
  clip_t        clips[],
  const bsp_t   *bsp,
  int           node_id,
  const hull_t  *hull,
  bsp_plane_t   min_plane,
  float         min_plane_dist,
  int           num_clips)
{
  if (node_id == -1) {
    return num_clips;
  }
  
  bsp_node_t *node = &bsp->nodes[node_id];
  
  float min_dist = hull_furthest_in(hull, node->plane.normal) - node->plane.distance;
  float max_dist = hull_furthest_out(hull, node->plane.normal) - node->plane.distance;
  
  if (max_dist > 0) {
    num_clips = bsp_clip_hull_R(clips, bsp, node->ahead, hull, min_plane, min_plane_dist, num_clips);
  }
  
  if (min_dist < 0) {
    if (min_dist > min_plane_dist && !node->bevel) {
      min_plane = node->plane;
      min_plane_dist = min_dist;
    }
    
    if (node->behind == -1) {
      clips[num_clips].normal = min_plane.normal;
      clips[num_clips].distance = min_plane.distance;
      num_clips++;
    }
    
    num_clips = bsp_clip_hull_R(clips, bsp, node->behind, hull, min_plane, min_plane_dist, num_clips);
  }
  
  return num_clips;
}

float hull_furthest_in(const hull_t *hull, vec3_t direction)
{
  float furthest_in = vec3_dot(vec3_add(hull->pos, hull->vertices[0]), direction);
  
  for (int i = 1; i < hull->num_vertices; i++) {
    float distance = vec3_dot(vec3_add(hull->pos, hull->vertices[i]), direction);
    
    if (distance < furthest_in) {
      furthest_in = distance;
    }
  }
  
  return furthest_in;
}

float hull_furthest_out(const hull_t *hull, vec3_t direction)
{
  float furthest_in = vec3_dot(vec3_add(hull->pos, hull->vertices[0]), direction);
  
  for (int i = 1; i < hull->num_vertices; i++) {
    float distance = vec3_dot(vec3_add(hull->pos, hull->vertices[i]), direction);
    
    if (distance > furthest_in) {
      furthest_in = distance;
    }
  }
  
  return furthest_in;
}
