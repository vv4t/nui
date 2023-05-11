#include "mesh_file.h"

#include "../common/log.h"
#include <stdio.h>
#include <stdlib.h>

bool mesh_file_load(mesh_file_t *mesh_file, const char *path)
{
  FILE *file = fopen(path, "rb");
  if (!file) {
    LOG_ERROR("failed to open '%s'", path);
    return false;
  }
  
  int num_faces; 
  fread(&num_faces, sizeof(int), 1, file);
  
  mesh_file->num_vertices = num_faces * 3;
  mesh_file->vertices = malloc(mesh_file->num_vertices * sizeof(vertex_t));
  
  for (int i = 0; i < num_faces; i++) {
    vec3_t pos[3];
    vec3_t normal[3];
    vec2_t uv[3];
    
    for (int j = 0; j < 3; j++) {
      fread(&pos[j], sizeof(vec3_t), 1, file);
      fread(&normal[j], sizeof(vec3_t), 1, file);
      fread(&uv[j], sizeof(vec2_t), 1, file);
      
      mesh_file->vertices[i * 3 + j].pos = pos[j];
      mesh_file->vertices[i * 3 + j].normal = normal[j];
      mesh_file->vertices[i * 3 + j].uv = uv[j];
    }
    
    vec3_t delta_pos1 = vec3_sub(pos[1], pos[0]);
    vec3_t delta_pos2 = vec3_sub(pos[2], pos[0]);
    
    vec2_t delta_uv1 = vec2_sub(uv[1], uv[0]);
    vec2_t delta_uv2 = vec2_sub(uv[2], uv[0]);
    
    float f = 1.0f / (delta_uv1.x * delta_uv2.y - delta_uv2.x * delta_uv1.y);
    vec3_t tangent = vec3_mulf(
      vec3_sub(
        vec3_mulf(delta_pos1, delta_uv2.y),
        vec3_mulf(delta_pos2, delta_uv1.y)
      ), f
    );
    
    vec3_t bitangent = vec3_mulf(
      vec3_sub(
        vec3_mulf(delta_pos2, delta_uv1.x),
        vec3_mulf(delta_pos1, delta_uv2.x)
      ), f
    );
    
    for (int j = 0; j < 3; j++) {
      mesh_file->vertices[i * 3 + j].tangent = vec3_normalize(tangent);
      mesh_file->vertices[i * 3 + j].bitangent = vec3_normalize(bitangent);
    }
  }
  
  return true;
}

void mesh_file_free(mesh_file_t *mesh_file)
{
  free(mesh_file->vertices);
}
