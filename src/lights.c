#include "lights.h"

#include "file.h"
#include "shader.h"

static bool lights_init_light_shader(lights_t *lights);
static bool lights_init_lights(lights_t *lights);

bool lights_init(lights_t *lights)
{
  if (!lights_init_light_shader(lights))
    return false;
  
  lights_init_lights(lights);
  
  return true;
}

void lights_bind(lights_t *lights)
{
  glUseProgram(lights->light_shader);
}

void lights_bind_material(material_t *material)
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, material->color);
  
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, material->normal);
}

void lights_set_light(lights_t *lights, int light_id, vec3_t pos, float intensity, vec4_t color)
{
  light_t light = {
    .pos = pos,
    .intensity = intensity,
    .color = color
  };
  
  glBindBuffer(GL_UNIFORM_BUFFER, lights->ubo_lights);
  glBufferSubData(GL_UNIFORM_BUFFER, light_id * sizeof(light_t), sizeof(light_t), &light);
}

static bool lights_init_lights(lights_t *lights)
{
  glGenBuffers(1, &lights->ubo_lights);
  glBindBuffer(GL_UNIFORM_BUFFER, lights->ubo_lights);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(ub_lights_t), NULL, GL_DYNAMIC_DRAW);
  
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, lights->ubo_lights); 
  
  ub_lights_t ub_lights = {0};
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ub_lights_t), &ub_lights);
}

static bool lights_init_light_shader(lights_t *lights)
{
  char *src_vertex = file_read_all("res/shader/lights.vert");
  char *src_fragment = file_read_all("res/shader/lights.frag");
  
  if (!shader_load(&lights->light_shader, src_vertex, NULL, src_fragment))
    return false;
  
  free(src_vertex);
  free(src_fragment);
  
  GLuint ubl_matrices = glGetUniformBlockIndex(lights->light_shader, "ubo_matrices");
  GLuint ubl_lights = glGetUniformBlockIndex(lights->light_shader, "ubo_lights");
  
  GLuint ul_color = glGetUniformLocation(lights->light_shader, "u_color");
  GLuint ul_normal = glGetUniformLocation(lights->light_shader, "u_normal");
  GLuint ul_depth_map = glGetUniformLocation(lights->light_shader, "u_depth_cube_map");
  
  glUseProgram(lights->light_shader);
  glUniform1i(ul_color, 0);
  glUniform1i(ul_normal, 1);
  // glUniform1i(ul_depth_map, 2);
  
  glUniformBlockBinding(lights->light_shader, ubl_matrices, 0);
  glUniformBlockBinding(lights->light_shader, ubl_lights, 1);
  
  return true;
}
