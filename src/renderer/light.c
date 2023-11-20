#include "light.h"

#include "shader.h"

typedef struct {
  GLuint shader;
  GLuint ul_view_pos;
  GLuint ubo_light;
} light_t;

static light_t light;

typedef struct {
  vec3_t    pos;
  float     intensity;
  vec4_t    color;
} ub_point_t;

typedef struct {
  ub_point_t points[4];
} ub_light_t;

static void light_init_uniform_location();
static void light_init_uniform_buffer();

bool light_init()
{
  if (!shader_load(&light.shader, "light")) {
    return false;
  }
  
  glUseProgram(light.shader);
  
  light_init_uniform_location();
  light_init_uniform_buffer();
  
  return true;
}

static void light_init_uniform_location()
{
  light.ul_view_pos = glGetUniformLocation(light.shader, "u_view_pos");
  
  GLuint ul_color = glGetUniformLocation(light.shader, "u_color");
  glUniform1i(ul_color, 0);
  
  GLuint ubl_matrices = glGetUniformBlockIndex(light.shader, "ubo_matrices");
  glUniformBlockBinding(light.shader, ubl_matrices, 0);
  
  GLuint ubl_lights = glGetUniformBlockIndex(light.shader, "ubo_lights");
  glUniformBlockBinding(light.shader, ubl_lights, 1);
}

static void light_init_uniform_buffer()
{
  glGenBuffers(1, &light.ubo_light);
  glBindBuffer(GL_UNIFORM_BUFFER, light.ubo_light);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(ub_light_t), NULL, GL_DYNAMIC_DRAW);
  
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, light.ubo_light); 
}

void light_sub_point(int id, vec3_t pos, float intensity, vec4_t color)
{
  ub_point_t point = {
    .pos = pos,
    .intensity = intensity,
    .color = color
  };
  
  glBindBuffer(GL_UNIFORM_BUFFER, light.ubo_light);
  glBufferSubData(GL_UNIFORM_BUFFER, id * sizeof(ub_point_t), sizeof(ub_point_t), &point);
}

void light_bind()
{
  glUseProgram(light.shader);
}
