#include <renderer/light.h>

typedef struct {
  vector position;
  vector color;
} ub_point_t;

typedef struct {
  ub_point_t point[POINT_MAX];
} ub_light_t;

struct {
  int num_point;
  GLuint ubo;
} light;

void light_init()
{
  static ub_light_t ub_light = {0};
  
  glGenBuffers(1, &light.ubo);
  glBindBuffer(GL_UNIFORM_BUFFER, light.ubo);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(ub_light_t), &ub_light, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, light.ubo);
  
  light.num_point = 0;
}

point_t light_add_point(vector position, vector color)
{
  point_t point = light.num_point++;
  
  if (point >= POINT_MAX) {
    return -1;
  }
  
  light_update_point(point, position, color);
  
  return point;
}

void light_update_point(point_t point, vector position, vector color)
{
  if (point < 0 || point >= POINT_MAX) {
    return;
  }
  
  ub_point_t ub_point = { .position = position, .color = color };
  glBindBuffer(GL_UNIFORM_BUFFER, light.ubo);
  glBufferSubData(GL_UNIFORM_BUFFER, point * sizeof(ub_point_t), sizeof(ub_point_t), &ub_point);
}

void light_shader_import(shaderdata_t sd)
{
  shaderdata_source(sd, "assets/shader/import/light.glsl", SD_FRAG);
}

void light_shader_attach(shader_t shader)
{
  shader_bind(shader);
  glUniformBlockBinding(shader, glGetUniformBlockIndex(shader, "ub_light"), 1);
}

void light_deinit()
{
  glDeleteBuffers(1, &light.ubo);
}
