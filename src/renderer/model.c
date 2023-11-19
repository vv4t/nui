#include "model.h"

#include "../common/log.h"
#include "../common/path.h"

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
  path_t path;
  int num_vertex_groups;
  mdl_vertex_group_t *vertex_groups;
  int num_vertices;
  mdl_vertex_t *vertices;
} mdl_file_t;

mdl_file_t *mdl_file_load(path_t path);
static void mdl_file_free(mdl_file_t *mdl_file);
static vertex_t *model_load_vertices(mdl_file_t *mdl_file);
static bool model_load_mesh_groups(model_t *model, mdl_file_t *mdl_file, const vertex_t *vertices);
static vertex_t *model_load_map_vertices(const map_t *map);
static bool model_load_map_mesh_groups(model_t *model, const vertex_t *vertices, const map_t *map);
static void vertex_solve_tangent(vertex_t *v1, vertex_t *v2, vertex_t *v3);
static void vertex_planar_map(vertex_t *v);

bool model_load(model_t *model, const char *name)
{
  path_t path;
  path_create(path, "assets/mdl/%s/%s.mdl", name, name);
  mdl_file_t *mdl_file = mdl_file_load(path);
  
  vertex_t *vertices = model_load_vertices(mdl_file);
  
  if (!model_load_mesh_groups(model, mdl_file, vertices)) {
    return false;
  }
  
  model->num_meshes = mdl_file->num_vertex_groups;
  
  mdl_file_free(mdl_file);
  free(vertices);
  
  return true;
}

void model_draw(const model_t *model)
{
  for (int i = 0; i < model->num_meshes; i++) {
    mesh_group_t mesh_group = model->mesh_groups[i];
    
    material_bind(mesh_group.material);
    mesh_draw(mesh_group.mesh);
  }
}

static bool model_load_mesh_groups(model_t *model, mdl_file_t *mdl_file, const vertex_t *vertices)
{
  for (int i = 0; i < mdl_file->num_vertex_groups; i++) {
    mesh_group_t *mesh_group = &model->mesh_groups[i];
    mdl_vertex_group_t vertex_group = mdl_file->vertex_groups[i];
    
    path_new(mdl_file->path, vertex_group.material.diffuse);
    
    if (!texture_load(&mesh_group->material.diffuse, mdl_file->path)) {
      return false;
    }
    
    const vertex_t *vertex_offset = &vertices[vertex_group.offset];
    
    if (!mesh_buffer_new(&mesh_group->mesh, vertex_offset, vertex_group.count)) {
      return false;
    }
  }
  
  return true;
}

static vertex_t *model_load_vertices(mdl_file_t *mdl_file)
{
  vertex_t *vertices = calloc(mdl_file->num_vertices, sizeof(vertex_t));
  
  for (int i = 0; i < mdl_file->num_vertices; i += 3) {
    for (int j = 0; j < 3; j++) {
      vertices[i + j].pos = mdl_file->vertices[i + j].pos;
      vertices[i + j].normal = mdl_file->vertices[i + j].normal;
      vertices[i + j].uv = mdl_file->vertices[i + j].uv;
    }
    
    vertex_solve_tangent(&vertices[i + 0], &vertices[i + 1], &vertices[i + 2]);
  }
  
  return vertices;
}

bool model_load_map(model_t *model, const map_t *map)
{
  vertex_t *vertices = model_load_map_vertices(map);
  
  if (!model_load_map_mesh_groups(model, vertices, map)) {
    return false;
  }
  
  model->num_meshes = map->num_vertex_groups;
  
  free(vertices);
  
  return true;
}

static vertex_t *model_load_map_vertices(const map_t *map)
{
  vertex_t *vertices = calloc(map->num_vertices, sizeof(vertex_t));
  
  for (int i = 0; i < map->num_vertices; i += 3) {
    for (int j = 0; j < 3; j++) {
      vertices[i + j].pos = map->vertices[i + j].pos;
      vertices[i + j].normal = map->vertices[i + j].normal;
      
      vertex_planar_map(&vertices[i + j]);
    }
    
    vertex_solve_tangent(&vertices[i + 0], &vertices[i + 1], &vertices[i + 2]);
  }
  
  return vertices;
}

static bool model_load_map_mesh_groups(model_t *model, const vertex_t *vertices, const map_t *map)
{
  for (int i = 0; i < map->num_vertex_groups; i++) {
    mesh_group_t *mesh_group = &model->mesh_groups[i];
    map_vertex_group_t vertex_group = map->vertex_groups[i];
    
    path_t path;
    path_copy(path, map->path);
    
    path_new(path, vertex_group.material.diffuse);
    
    if (!texture_load(&mesh_group->material.diffuse, path)) {
      return false;
    }
    
    const vertex_t *vertex_offset = &vertices[vertex_group.offset];
    
    if (!mesh_buffer_new(&mesh_group->mesh, vertex_offset, vertex_group.count)) {
      return false;
    }
  }
  
  return true;
}

static void vertex_planar_map(vertex_t *v)
{
  if (fabs(v->normal.x) > fabs(v->normal.y)) {
    if (fabs(v->normal.z) > fabs(v->normal.x)) {
      v->uv = vec2_init(v->pos.x, v->pos.y);
    } else {
      v->uv = vec2_init(v->pos.z, v->pos.y);
    }
  } else {
    if (fabs(v->normal.z) > fabs(v->normal.y)) {
      v->uv = vec2_init(v->pos.x, v->pos.y);
    } else {
      v->uv = vec2_init(v->pos.x, v->pos.z);
    }
  }
}

static void vertex_solve_tangent(vertex_t *v1, vertex_t *v2, vertex_t *v3)
{
  vec3_t d_pos1 = vec3_sub(v2->pos, v1->pos);
  vec3_t d_pos2 = vec3_sub(v3->pos, v1->pos);
  
  vec2_t d_uv1 = vec2_sub(v2->uv, v1->uv);
  vec2_t d_uv2 = vec2_sub(v3->uv, v1->uv);
  
  float f = 1.0f / (d_uv1.x * d_uv2.y - d_uv2.x * d_uv1.y);
  
  vec3_t tangent = vec3_mulf(vec3_sub(vec3_mulf(d_pos1, d_uv2.y), vec3_mulf(d_pos2, d_uv1.y)), f);
  vec3_t bitangent = vec3_mulf(vec3_sub(vec3_mulf(d_pos2, d_uv1.x), vec3_mulf(d_pos1, d_uv2.x)), f);
  
  v1->tangent = tangent;
  v2->tangent = tangent;
  v3->tangent = tangent;
  
  v1->bitangent = bitangent;
  v2->bitangent = bitangent;
  v3->bitangent = bitangent;
}

mdl_file_t *mdl_file_load(path_t path)
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
  path_copy(mdl_file->path, path);
  
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
