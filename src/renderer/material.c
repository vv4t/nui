#include "material.h"

#include "api.h"

typedef struct {
  vec3_t color;
  float specular;
  float alpha;
} ub_material_t;

typedef struct {
  GLuint empty_diffuse;
  GLuint empty_normal;
  GLuint ubo_material;
} material_def_t;

static material_def_t material_def;

void diffuse_map_init();
void normal_map_init();

void material_init()
{
  diffuse_map_init();
  normal_map_init();
  
  glGenBuffers(1, &material_def.ubo_material);
  glBindBuffer(GL_UNIFORM_BUFFER, material_def.ubo_material);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(ub_material_t), NULL, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, UBO_MATERIAL_BINDING, material_def.ubo_material);
}

void diffuse_map_init()
{
  float data[] = { 1.0, 1.0, 1.0, 1.0 };
  
  glGenTextures(1, &material_def.empty_diffuse);
  glBindTexture(GL_TEXTURE_2D, material_def.empty_diffuse);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1, 1, 0, GL_RGBA, GL_FLOAT, data);
}

void normal_map_init()
{
  float data[] = { 0.5, 0.5, 1.0, 1.0 };
  
  glGenTextures(1, &material_def.empty_normal);
  glBindTexture(GL_TEXTURE_2D, material_def.empty_normal);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1, 1, 0, GL_RGBA, GL_FLOAT, data);
}

void material_new(material_t *material)
{
  material->diffuse = material_def.empty_diffuse;
  material->normal = material_def.empty_normal;
  material->color = vec3_init(1.0, 1.0, 1.0);
  material->specular = 1.0;
  material->alpha = 1.0;
}

void material_bind(material_t material)
{
  ub_material_t ub_material = {
    .color = material.color,
    .specular = material.specular,
    .alpha = material.alpha
  };
  
  glBindBuffer(GL_UNIFORM_BUFFER, material_def.ubo_material);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ub_material), &ub_material);
  
  glActiveTexture(GL_TEXTURE0 + TEXTURE_DIFFUSE_BINDING);
  glBindTexture(GL_TEXTURE_2D, material.diffuse);
  
  glActiveTexture(GL_TEXTURE0 + TEXTURE_NORMAL_BINDING);
  glBindTexture(GL_TEXTURE_2D, material.normal);
}

void material_shader_setup(GLuint shader)
{
  glUseProgram(shader);
  
  GLuint ul_color = glGetUniformLocation(shader, "u_color");
  glUniform1i(ul_color, TEXTURE_DIFFUSE_BINDING);
  
  GLuint ul_normal = glGetUniformLocation(shader, "u_normal");
  glUniform1i(ul_normal, TEXTURE_NORMAL_BINDING);
  
  GLuint ubl_material = glGetUniformBlockIndex(shader, "ub_material");
  glUniformBlockBinding(shader, ubl_material, UBO_MATERIAL_BINDING);
}
