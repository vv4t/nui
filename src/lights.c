#include "lights.h"

#include "file.h"

static bool lights_init_light_shader(lights_t *lights);
static bool lights_init_shadow_shader(lights_t *lights);
static bool lights_init_lights(lights_t *lights);
static bool lights_init_shadow(lights_t *lights);

bool lights_init(lights_t *lights)
{
  if (!lights_init_light_shader(lights))
    return false;
  
  if (!lights_init_shadow_shader(lights))
    return false;
  
  lights_init_lights(lights);
  lights_init_shadow(lights);
  
  return true;
}

void lights_bind(lights_t *lights)
{
  glUseProgram(lights->light_shader);
  
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, lights->depth_map);
}

void lights_bind_material(material_t *material)
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, material->color);
  
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, material->normal);
}

void lights_set_light(
  lights_t    *lights,
  int         light_id,
  draw_call_t *draw_call,
  vec3_t      pos,
  float       intensity,
  vec4_t      color)
{
  mat4x4_t projection_matrix = mat4x4_init_perspective(1.0, to_radians(90), 0.1, 100.0);
  
  glBindFramebuffer(GL_FRAMEBUFFER, lights->depth_fbo);
  glUseProgram(lights->shadow_shader);
  
  glClear(GL_DEPTH_BUFFER_BIT);
  
  mat4x4_t view_matrices[] = {
    mat4x4_init_look_at(vec3_init(1.0, 0.0, 0.0), pos, vec3_init(0.0, 1.0, 0.0)),
    mat4x4_init_look_at(vec3_init(0.0, 1.0, 0.0), pos, vec3_init(0.0, 0.0, -1.0)),
    mat4x4_init_look_at(vec3_init(0.0, 0.0, 1.0), pos, vec3_init(0.0, 1.0, 0.0)),
    mat4x4_init_look_at(vec3_init(-1.0, 0.0, 0.0), pos, vec3_init(0.0, 1.0, 0.0)),
    mat4x4_init_look_at(vec3_init(0.0, -1.0, 0.0), pos, vec3_init(0.0, 0.0, 1.0)),
    mat4x4_init_look_at(vec3_init(0.0, 0.0, -1.0), pos, vec3_init(0.0, 1.0, 0.0))
  };
  
  light_t light = {
    .pos = pos,
    .intensity = intensity,
    .color = color
  };
  
  for (int i = 0; i < 6; i++) {
    mat4x4_t bias_matrix = mat4x4_init(
      vec4_init(0.5 / 6.0, 0.0, 0.0, 0.5 / 6.0),
      vec4_init(0.0, 0.5, 0.0, 0.5),
      vec4_init(0.0, 0.0, 0.5, 0.5),
      vec4_init(0.0, 0.0, 0.0, 1.0)
    );
    
    mat4x4_t view_projection_matrix = mat4x4_mul(view_matrices[i], projection_matrix);
    mat4x4_t bias_mvp = mat4x4_mul(view_projection_matrix, bias_matrix);
    
    light.light_matrices[i] = bias_mvp;
    
    glViewport(i * 1024, 0, 1024, 1024);
    do_draw_call(draw_call, view_projection_matrix, pos);
  }
  
  glBindBuffer(GL_UNIFORM_BUFFER, lights->ubo_lights);
  glBufferSubData(GL_UNIFORM_BUFFER, light_id * sizeof(light_t), sizeof(light_t), &light);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static bool lights_init_shadow(lights_t *lights)
{
  glGenTextures(1, &lights->depth_map);
  glBindTexture(GL_TEXTURE_2D, lights->depth_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 6 * 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  
  float border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
  
  glGenFramebuffers(1, &lights->depth_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, lights->depth_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, lights->depth_map, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);  
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
  
  if (!shader_load(&lights->light_shader, src_vertex, src_fragment))
    return false;
  
  free(src_vertex);
  free(src_fragment);
  
  GLuint ubl_matrices = glGetUniformBlockIndex(lights->light_shader, "ubo_matrices");
  GLuint ubl_lights = glGetUniformBlockIndex(lights->light_shader, "ubo_lights");
  
  GLuint ul_color = glGetUniformLocation(lights->light_shader, "u_color");
  GLuint ul_normal = glGetUniformLocation(lights->light_shader, "u_normal");
  GLuint ul_depth_map = glGetUniformLocation(lights->light_shader, "u_depth_map");
  
  glUseProgram(lights->light_shader);
  glUniform1i(ul_color, 0);
  glUniform1i(ul_normal, 1);
  glUniform1i(ul_depth_map, 2);
  
  glUniformBlockBinding(lights->light_shader, ubl_matrices, 0);
  glUniformBlockBinding(lights->light_shader, ubl_lights, 1);
  
  return true;
}

static bool lights_init_shadow_shader(lights_t *lights)
{
  char *src_vertex = file_read_all("res/shader/shadow.vert");
  char *src_fragment = file_read_all("res/shader/shadow.frag");
  
  if (!shader_load(&lights->shadow_shader, src_vertex, src_fragment))
    return false;
  
  free(src_vertex);
  free(src_fragment);
  
  GLuint ubl_matrices = glGetUniformBlockIndex(lights->shadow_shader, "ubo_matrices");
  glUniformBlockBinding(lights->shadow_shader, ubl_matrices, 0);
  
  return true;
}
