#include "model.h"

#include "../common/log.h"

#define PATH_LEN 256

typedef struct {
  char diffuse[64];
} mdl_material_t;

typedef struct {
  mdl_material_t material;
  int offset;
  int count;
} mdl_vertex_group_t;

typedef struct {
  vec3_t pos;
  vec3_t normal;
  vec2_t uv;
} mdl_vertex_t;

typedef struct {
  int                 num_vertex_groups;
  mdl_vertex_group_t  *vertex_groups;
  
  int                 num_vertices;
  mdl_vertex_t        *vertices;
} mdl_file_t;

mdl_file_t      *mdl_file_load(const char *path);
void            mdl_file_free(mdl_file_t *mesh_file);
static vertex_t *load_model_vertices(mdl_file_t *mdl_file);

bool model_load(model_t *model, mesh_buffer_t *mesh_buffer, const char *name)
{
  char path[PATH_LEN];
  strncpy(path, "assets/mdl/", PATH_LEN);
  strcat(path, name);
  strcat(path, "/");
  
  int base_len = strlen(path);
  
  strcat(path, name);
  strcat(path, ".mdl");
  
  mdl_file_t *mdl_file = mdl_file_load(path);
  path[base_len] = 0;
  
  vertex_t *vertices = load_model_vertices(mdl_file);
  
  for (int i = 0; i < mdl_file->num_vertex_groups; i++) {
    mesh_group_t *mesh_group = &model->mesh_groups[i];
    mdl_vertex_group_t vertex_group = mdl_file->vertex_groups[i];
    
    strcat(path, vertex_group.material.diffuse);
    
    if (!texture_load(&mesh_group->material.diffuse, path)) {
      return false;
    }
    
    path[base_len] = 0;
    
    if (
      !mesh_buffer_new(
        mesh_buffer,
        &mesh_group->mesh,
        &vertices[vertex_group.offset],
        vertex_group.count
      )
    ) {
      return false;
    }
  }
  
  model->num_meshes = mdl_file->num_vertex_groups;
  
  mdl_file_free(mdl_file);
  free(vertices);
  
  return true;
}

bool model_load_map(model_t *model, mesh_buffer_t *mesh_buffer, map_t *map)
{
  vertex_t *vertices = calloc(map->num_vertices, sizeof(vertex_t));
  
  for (int i = 0; i < map->num_vertices; i += 3) {
    for (int j = 0; j < 3; j++) {
      vec3_t pos = map->vertices[i + j].pos;
      vec3_t normal = map->vertices[i + j].normal;
      vec2_t uv;
      
      if (fabs(normal.x) > fabs(normal.y)) {
        if (fabs(normal.z) > fabs(normal.x)) {
          uv = vec2_init(pos.x, pos.y);
        } else {
          uv = vec2_init(pos.z, pos.y);
        }
      } else {
        if (fabs(normal.z) > fabs(normal.y)) {
          uv = vec2_init(pos.x, pos.y);
        } else {
          uv = vec2_init(pos.x, pos.z);
        }
      }
      
      vertices[i + j].pos = pos;
      vertices[i + j].normal = normal;
      vertices[i + j].uv = uv;
    }
    
    vec3_t delta_pos1 = vec3_sub(vertices[i + 1].pos, vertices[i].pos);
    vec3_t delta_pos2 = vec3_sub(vertices[i + 2].pos, vertices[i].pos);
    
    vec2_t delta_uv1 = vec2_sub(vertices[i + 1].uv, vertices[i].uv);
    vec2_t delta_uv2 = vec2_sub(vertices[i + 1].uv, vertices[i].uv);
    
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
      vertices[i + j].tangent = vec3_normalize(tangent);
      vertices[i + j].bitangent = vec3_normalize(bitangent);
    }
  }
  
  for (int i = 0; i < map->num_vertex_groups; i++) {
    mesh_group_t *mesh_group = &model->mesh_groups[i];
    map_vertex_group_t vertex_group = map->vertex_groups[i];
    
    if (!texture_load(&mesh_group->material.diffuse, vertex_group.material.diffuse)) {
      return false;
    }
    
    if (
      !mesh_buffer_new(
        mesh_buffer,
        &mesh_group->mesh,
        &vertices[vertex_group.offset],
        vertex_group.count
      )
    ) {
      return false;
    }
  }
  
  model->num_meshes = map->num_vertex_groups;
  
  free(vertices);
  
  return true;
}

static vertex_t *load_model_vertices(mdl_file_t *mdl_file)
{
  vertex_t *vertices = calloc(mdl_file->num_vertices, sizeof(vertex_t));
  
  for (int i = 0; i < mdl_file->num_vertices; i += 3) {
    for (int j = 0; j < 3; j++) {
      vertices[i + j].pos = mdl_file->vertices[i + j].pos;
      vertices[i + j].normal = mdl_file->vertices[i + j].normal;
      vertices[i + j].uv = mdl_file->vertices[i + j].uv;
    }
    
    vec3_t delta_pos1 = vec3_sub(vertices[i + 1].pos, vertices[i].pos);
    vec3_t delta_pos2 = vec3_sub(vertices[i + 2].pos, vertices[i].pos);
    
    vec2_t delta_uv1 = vec2_sub(vertices[i + 1].uv, vertices[i].uv);
    vec2_t delta_uv2 = vec2_sub(vertices[i + 1].uv, vertices[i].uv);
    
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
      vertices[i + j].tangent = vec3_normalize(tangent);
      vertices[i + j].bitangent = vec3_normalize(bitangent);
    }
  }
  
  return vertices;
}

void model_draw(const model_t *model)
{
  for (int i = 0; i < model->num_meshes; i++) {
    mesh_group_t mesh_group = model->mesh_groups[i];
    
    material_bind(mesh_group.material);
    mesh_draw(mesh_group.mesh);
  }
}

mdl_file_t *mdl_file_load(const char *path)
{
  FILE *file = fopen(path, "rb");
  if (!file) {
    LOG_ERROR("failed to open '%s'", path);
    return NULL;
  }
  
  mdl_file_t *mdl_file = malloc(sizeof(mdl_file_t));
  
  fread(&mdl_file->num_vertex_groups, sizeof(int), 1, file); 
  fread(&mdl_file->num_vertices, sizeof(int), 1, file); 
  
  mdl_file->vertex_groups = calloc(mdl_file->num_vertex_groups, sizeof(mdl_vertex_group_t));
  mdl_file->vertices = calloc(mdl_file->num_vertices, sizeof(mdl_vertex_t));
  
  fread(mdl_file->vertex_groups, sizeof(mdl_vertex_group_t), mdl_file->num_vertex_groups, file);
  fread(mdl_file->vertices, sizeof(mdl_vertex_t), mdl_file->num_vertices, file);
  
  return mdl_file;
}

void mdl_file_free(mdl_file_t *mdl_file)
{
  free(mdl_file->vertex_groups);
  free(mdl_file->vertices);
  free(mdl_file);
}
