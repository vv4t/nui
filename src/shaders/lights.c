#include "lights.h"

#include "../common/file.h"

static bool lights_init_light_shader(lights_t *lights);
static bool lights_init_shadow_shader(lights_t *lights);
static void lights_init_lights(lights_t *lights);
static void lights_init_shadow(lights_t *lights);

bool lights_init(lights_t *lights)
{
  if (!lights_init_light_shader(lights))
    return false;
  
  if (!lights_init_shadow_shader(lights))
    return false;
  
  lights_init_lights(lights);
  lights_init_shadow(lights);
  
  lights->light_count = 0;
  
  return true;
}

void lights_bind(lights_t *lights)
{
  glUseProgram(lights->light_shader);
  
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, lights->depth_map);
}

void lights_set_scene(lights_t *lights, scene_t *scene)
{
  lights->scene = scene;
}

void lights_set_view_pos(lights_t *lights, vec3_t view_pos)
{
  glUniform3f(lights->ul_view_pos, view_pos.x, view_pos.y, view_pos.z);
}

void lights_set_material(material_t *material)
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, material->diffuse);
  
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, material->normal);
}

bool lights_new_light(lights_t *lights, light_t *light)
{
  *light = (light_t) {
    .id = lights->light_count,
    .pos = vec3_init(0.0, 0.0, 0.0),
    .color = vec4_init(1.0, 1.0, 1.0, 1.0),
    .intensity = 5.0
  };
  
  lights->light_count++;
  
  return true;
}

void lights_sub_light(lights_t *lights, light_t *light)
{
  glBindFramebuffer(GL_FRAMEBUFFER, lights->depth_fbo);
  
  glViewport(0, light->id * 1024, 6 * 1024, 1024);
  glScissor(0, light->id * 1024, 6 * 1024, 1024);
  glEnable(GL_SCISSOR_TEST);
  glClear(GL_DEPTH_BUFFER_BIT);
  glDisable(GL_SCISSOR_TEST);
  
  glUseProgram(lights->shadow_shader);
  
  mat4x4_t projection_matrix = mat4x4_init_perspective(1.0, to_radians(90), 0.1, 100.0);
  
  mat4x4_t view_matrices[] = {
    mat4x4_init_look_at(vec3_add(vec3_init( 1.0,  0.0,  0.0), light->pos), light->pos, vec3_init(0.0, -1.0,  0.0)),
    mat4x4_init_look_at(vec3_add(vec3_init(-1.0,  0.0,  0.0), light->pos), light->pos, vec3_init(0.0, -1.0,  0.0)),
    mat4x4_init_look_at(vec3_add(vec3_init( 0.0,  1.0,  0.0), light->pos), light->pos, vec3_init(0.0,  0.0,  1.0)),
    mat4x4_init_look_at(vec3_add(vec3_init( 0.0, -1.0,  0.0), light->pos), light->pos, vec3_init(0.0,  0.0, -1.0)),
    mat4x4_init_look_at(vec3_add(vec3_init( 0.0,  0.0,  1.0), light->pos), light->pos, vec3_init(0.0, -1.0,  0.0)),
    mat4x4_init_look_at(vec3_add(vec3_init( 0.0,  0.0, -1.0), light->pos), light->pos, vec3_init(0.0, -1.0,  0.0))
  };
  
  mat4x4_t bias_matrix = mat4x4_init(
    vec4_init(0.5 / 6.0,  0.0,              0.0, 0.5 / 6.0),
    vec4_init(0.0,        0.5 / MAX_LIGHTS, 0.0, 0.5 / MAX_LIGHTS),
    vec4_init(0.0,        0.0,              0.5, 0.5),
    vec4_init(0.0,        0.0,              0.0, 1.0)
  );
  
  ubc_light_t ubc_light = {
    .pos = light->pos,
    .intensity = light->intensity,
    .color = light->color
  };
  
  for (int i = 0; i < 6; i++) {
    mat4x4_t view_projection_matrix = mat4x4_mul(view_matrices[i], projection_matrix);
    mat4x4_t bias_mvp = mat4x4_mul(view_projection_matrix, bias_matrix);
    
    ubc_light.light_matrices[i] = bias_mvp;
    
    glViewport(i * 1024, light->id * 1024, 1024, 1024);
    view_set(lights->scene->view, view_projection_matrix);
    draw_scene(lights->scene);
  }
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  glBindBuffer(GL_UNIFORM_BUFFER, lights->ubo_lights);
  glBufferSubData(GL_UNIFORM_BUFFER, light->id * sizeof(ubc_light_t), sizeof(ubc_light_t), &ubc_light);
}

static void lights_init_shadow(lights_t *lights)
{
  glGenTextures(1, &lights->depth_map);
  glBindTexture(GL_TEXTURE_2D, lights->depth_map);
  // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 6 * 1024, MAX_LIGHTS * 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, 6 * 1024, MAX_LIGHTS * 1024);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  glGenFramebuffers(1, &lights->depth_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, lights->depth_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, lights->depth_map, 0);
#ifndef __EMSCRIPTEN__
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
#endif
  glBindFramebuffer(GL_FRAMEBUFFER, 0);  
}

static void lights_init_lights(lights_t *lights)
{
  glGenBuffers(1, &lights->ubo_lights);
  glBindBuffer(GL_UNIFORM_BUFFER, lights->ubo_lights);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(ubc_lights_t), NULL, GL_DYNAMIC_DRAW);
  
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, lights->ubo_lights); 
  
  ubc_lights_t ubc_lights = {0};
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ubc_lights_t), &ubc_lights);
}

static bool lights_init_light_shader(lights_t *lights)
{
  char define[64];
  sprintf(define, "#define MAX_LIGHTS %i", MAX_LIGHTS);
  
  char *src_vertex = file_read_all("res/shader/lights.vsh");
  char *src_fragment = file_read_all("res/shader/lights.fsh");
  
  if (!shader_load(&lights->light_shader, define, src_vertex, src_fragment))
    return false;
  
  free(src_vertex);
  free(src_fragment);
  
  GLuint ubl_matrices = glGetUniformBlockIndex(lights->light_shader, "ubo_matrices");
  GLuint ubl_lights = glGetUniformBlockIndex(lights->light_shader, "ubo_lights");
  
  GLuint ul_color = glGetUniformLocation(lights->light_shader, "u_color");
  GLuint ul_normal = glGetUniformLocation(lights->light_shader, "u_normal");
  GLuint ul_depth_map = glGetUniformLocation(lights->light_shader, "u_depth_map");
  
  lights->ul_view_pos = glGetUniformLocation(lights->light_shader, "u_view_pos");
  
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
  char *src_vertex = file_read_all("res/shader/shadow.vsh");
  
  if (!shader_load(&lights->shadow_shader, "", src_vertex, "void main(){}"))
    return false;
  
  free(src_vertex);
  
  GLuint ubl_matrices = glGetUniformBlockIndex(lights->shadow_shader, "ubo_matrices");
  glUniformBlockBinding(lights->shadow_shader, ubl_matrices, 0);
  
  return true;
}
