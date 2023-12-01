#include "light.h"

#define CUBE_FACES 6
#define SHADOW_SIZE 1024
#define MAX_POINTS 4

#include "shader.h"

typedef struct {
  GLuint shader;
  GLuint ul_view_pos;
  GLuint ubo_light;
} light_t;

static light_t light;

typedef struct {
  GLuint shader;
  GLuint depth_map;
  GLuint depth_fbo;
} shadow_t;

static shadow_t shadow;

typedef struct {
  vec3_t pos;
  float intensity;
  vec4_t color;
} ub_point_t;

typedef struct {
  mat4x4_t light_matrices[CUBE_FACES];
} ub_point_shadow_t;

typedef struct {
  ub_point_t points[MAX_POINTS];
  ub_point_shadow_t point_shadows[MAX_POINTS];
} ub_light_t;

static bool shadow_init();

static void light_init_uniform_location();
static void light_init_uniform_buffer();

bool light_init()
{
  if (!shader_load(&light.shader, "light")) {
    return false;
  }
  
  light_init_uniform_buffer();
  light_init_uniform_location();
  
  if (!shadow_init()) {
    return false;
  }
  
  return true;
}

static void light_init_uniform_location()
{
  glUseProgram(light.shader);
  
  light.ul_view_pos = glGetUniformLocation(light.shader, "u_view_pos");
  
  GLuint ul_color = glGetUniformLocation(light.shader, "u_color");
  glUniform1i(ul_color, 0);
  
  GLuint ubl_matrices = glGetUniformBlockIndex(light.shader, "ubo_matrices");
  glUniformBlockBinding(light.shader, ubl_matrices, 0);
  
  GLuint ubl_lights = glGetUniformBlockIndex(light.shader, "ub_light");
  glUniformBlockBinding(light.shader, ubl_lights, 1);
}

static void light_init_uniform_buffer()
{
  glUseProgram(light.shader);
  
  glGenBuffers(1, &light.ubo_light);
  glBindBuffer(GL_UNIFORM_BUFFER, light.ubo_light);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(ub_light_t), NULL, GL_DYNAMIC_DRAW);
  
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, light.ubo_light); 
}

static bool shadow_init()
{
  glGenTextures(1, &shadow.depth_map);
  glBindTexture(GL_TEXTURE_2D, shadow.depth_map);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, CUBE_FACES * SHADOW_SIZE, MAX_POINTS * SHADOW_SIZE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  glGenFramebuffers(1, &shadow.depth_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, shadow.depth_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow.depth_map, 0);

#ifndef __EMSCRIPTEN__
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
#endif

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  if (!shader_load(&shadow.shader, "shadow")) {
    return false;
  }
  
  return true;
}

void light_sub_view_pos(vec3_t view_pos)
{
  glUniform3f(light.ul_view_pos, view_pos.x, view_pos.y, view_pos.z);
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

void light_update_point_shadow(int id, vec3_t pos)
{
  /*
  glBindFramebuffer(GL_FRAMEBUFFER, shadow.depth_fbo);
  glUseProgram(shadow.shader);
  
  vec3_t at[] = {
    vec3_init( 1.0,  0.0,  0.0)
    vec3_init(-1.0,  0.0,  0.0)
    vec3_init( 0.0,  1.0,  0.0)
    vec3_init( 0.0, -1.0,  0.0)
    vec3_init( 0.0,  0.0,  1.0)
    vec3_init( 0.0,  0.0, -1.0)
  };
  
  vec3_t up[] = {
    vec3_init(0.0, -1.0,  0.0)
    vec3_init(0.0, -1.0,  0.0)
    vec3_init(0.0,  0.0,  1.0)
    vec3_init(0.0,  0.0, -1.0)
    vec3_init(0.0, -1.0,  0.0)
    vec3_init(0.0, -1.0,  0.0)
  };
  
  view_t view;
  view_init_perspective(&renderer.view, to_radians(90.0), 0.1, 100.0);
  
  camera_set_view(view);
  
  for (int i = 0; i < 6; i++) {
    camera_look_at(
    
    view_set_matrix(&view, view_matrices[i]);
    ubc_light.light_matrices[i] = mat4x4_mul(view.view_projection_matrix, bias_matrix);
    glViewport(i * 1024, light->id * 1024, 1024, 1024);
    renderer_shadow_pass();
  }
  */
}

void light_bind()
{
  glUseProgram(light.shader);
}

/*
void light_update_point_shadow(int id, vec3_t pos)
{
  glBindFramebuffer(GL_FRAMEBUFFER, shadow.depth_fbo);
  
  // glViewport(0, light->id * 1024, 6 * 1024, 1024);
  glScissor(0, light->id * 1024, 6 * 1024, 1024);
  glEnable(GL_SCISSOR_TEST);
  glClear(GL_DEPTH_BUFFER_BIT);
  glDisable(GL_SCISSOR_TEST);
  
  glUseProgram(shadow.shader);
  
  mat4x4_t projection_matrix = mat4x4_init_perspective(1.0, to_radians(90), 0.1, 100.0);
  
  mat4x4_t view_matrices[] = {
    mat4x4_init_look_at(vec3_add(vec3_init( 1.0,  0.0,  0.0), pos), pos, vec3_init(0.0, -1.0,  0.0)),
    mat4x4_init_look_at(vec3_add(vec3_init(-1.0,  0.0,  0.0), pos), pos, vec3_init(0.0, -1.0,  0.0)),
    mat4x4_init_look_at(vec3_add(vec3_init( 0.0,  1.0,  0.0), pos), pos, vec3_init(0.0,  0.0,  1.0)),
    mat4x4_init_look_at(vec3_add(vec3_init( 0.0, -1.0,  0.0), pos), pos, vec3_init(0.0,  0.0, -1.0)),
    mat4x4_init_look_at(vec3_add(vec3_init( 0.0,  0.0,  1.0), pos), pos, vec3_init(0.0, -1.0,  0.0)),
    mat4x4_init_look_at(vec3_add(vec3_init( 0.0,  0.0, -1.0), pos), pos, vec3_init(0.0, -1.0,  0.0))
  };
  
  mat4x4_t bias_matrix = mat4x4_init(
    vec4_init(0.5 / 6.0,  0.0,              0.0, 0.5 / 6.0),
    vec4_init(0.0,        0.5 / MAX_POINTS, 0.0, 0.5 / MAX_POINTS),
    vec4_init(0.0,        0.0,              0.5, 0.5),
    vec4_init(0.0,        0.0,              0.0, 1.0)
  );
  
  view_t view;
  view_perspective(&view, mat4x4_init_perspective(1.0, to_radians(90.0), 0.1, 100.0));
  
  camera_set_view(view);
  
  for (int i = 0; i < 6; i++) {
    camera_view_space(view_matrices[i]);
    
    view_set_matrix(&view, view_matrices[i]);
    ubc_light.light_matrices[i] = mat4x4_mul(view.view_projection_matrix, bias_matrix);
    glViewport(i * 1024, light->id * 1024, 1024, 1024);
    renderer_shadow_pass();
  }
}*/
