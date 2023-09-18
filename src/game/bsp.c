#include "bsp.h"

int bsp_clip_R(clip_t clips[], const bsp_t *bsp, int node_id, const sphere_t *sphere, bsp_plane_t min_plane);

int bsp_clip_sphere(clip_t clips[], const bsp_t *bsp, const sphere_t *sphere)
{
  return bsp_clip_R(clips, bsp, 0, sphere, (bsp_plane_t) { .distance = -1000 });
}

int bsp_clip_R(clip_t clips[], const bsp_t *bsp, int node_id, const sphere_t *sphere, bsp_plane_t min_plane)
{
  if (node_id == -1) {
    return 0;
  }
  
  bsp_node_t *node = &bsp->nodes[node_id];
  
  float min_dist = vec3_dot(sphere->pos, node->plane.normal) - node->plane.distance - sphere->radius;
  float max_dist = vec3_dot(sphere->pos, node->plane.normal) - node->plane.distance + sphere->radius;
  
  int num_clips = 0;
  
  if (max_dist > 0) {
    num_clips += bsp_clip_R(clips, bsp, node->ahead, sphere, min_plane);
  }
  
  if (min_dist < 0) {
    if (min_dist > min_plane.distance) {
      min_plane.distance = min_dist;
      min_plane.normal = node->plane.normal;
    }
    
    if (node->behind == -1) {
      clips[num_clips++].normal = min_plane.normal;
    }
    
    num_clips += bsp_clip_R(clips, bsp, node->behind, sphere, min_plane);
  }
  
  return num_clips;
}
