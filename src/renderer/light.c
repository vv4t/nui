#include <renderer/light.h>

#define POINT_MAX 8

typedef struct {
  vector position;
  vector color;
} ub_point_t;

typedef struct {
  ub_point_t point[POINT_MAX];
} ub_light_t;

struct {
  GLuint ubo;
} light;

void light_init()
{
  static ub_light_t ub_light = {0};
  
  glGenBuffers(1, &light.ubo);
  glBindBuffer(GL_UNIFORM_BUFFER, light.ubo);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(ub_light_t), &ub_light, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, light.ubo);
  
  ub_point_t point[2] = {
    { .position = vec3(-4.0, 0.1, 5.0), .color = vec4(1.0, 0.5, 0.5, 32.0) },
    { .position = vec3(+4.0, 4.5, 0.0), .color = vec4(0.5, 0.5, 1.0, 32.0) }
  };
  
  glBindBuffer(GL_UNIFORM_BUFFER, light.ubo);
  glBufferSubData(GL_UNIFORM_BUFFER, 0 * sizeof(ub_point_t), 2 * sizeof(ub_point_t), &point);
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
