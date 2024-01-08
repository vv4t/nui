#include "material.h"

typedef struct {
  vec3_t color;
  float specular;
} ub_material_t;

typedef struct {
  GLuint empty_diffuse;
  GLuint ubo_material;
} material_def_t;

static material_def_t material_def;

void material_init()
{
  float data[] = { 1.0, 1.0, 1.0, 1.0 };
  
  glGenTextures(1, &material_def.empty_diffuse);
  glBindTexture(GL_TEXTURE_2D, material_def.empty_diffuse);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1, 1, 0, GL_RGBA, GL_FLOAT, data);
  
  glGenBuffers(1, &material_def.ubo_material);
  glBindBuffer(GL_UNIFORM_BUFFER, material_def.ubo_material);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(ub_material_t), NULL, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, material_def.ubo_material);
}

void material_new(material_t *material)
{
  material->diffuse = material_def.empty_diffuse;
  material->color = vec3_init(1.0, 1.0, 1.0);
  material->specular = 1.0;
}

void material_bind(material_t material)
{
  ub_material_t ub_material = {
    .color = material.color,
    .specular = material.specular
  };
  
  glBindBuffer(GL_UNIFORM_BUFFER, material_def.ubo_material);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ub_material), &ub_material);
  
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, material.diffuse);
}
