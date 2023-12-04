#include "light.h"

#define CUBE_FACES 6
#define POINTS_MAX 2
#define SHADOW_SIZE 1024
#define SHADOWS_MAX (POINTS_MAX * CUBE_FACES)

#include "shader.h"
#include "camera.h"
#include "renderer_api.h"
#include "../common/log.h"

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
  mat4x4_t light_matrix;
} ub_shadow_t;

typedef struct {
  ub_point_t points[POINTS_MAX];
  ub_shadow_t shadows[SHADOWS_MAX];
} ub_light_t;

static bool shadow_init();

static void light_init_uniform_location();
static void light_init_uniform_buffer();
static void light_update_point_shadow(int id, vec3_t pos);

bool light_init()
{
  if (!shader_load(&light.shader, "light")) {
    return false;
  }
  
  light_init_uniform_location();
  light_init_uniform_buffer();
  
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
  GLuint ul_depth_map = glGetUniformLocation(light.shader, "u_depth_map");
  
  glUniform1i(ul_color, 0);
  glUniform1i(ul_depth_map, 1);
  
  GLuint ubl_matrices = glGetUniformBlockIndex(light.shader, "ubo_matrices");
  GLuint ubl_lights = glGetUniformBlockIndex(light.shader, "ub_light");
  
  glUniformBlockBinding(light.shader, ubl_matrices, 0);
  glUniformBlockBinding(light.shader, ubl_lights, 1);
}

static void light_init_uniform_buffer()
{
  ub_light_t ub_light = {0};
  
  glGenBuffers(1, &light.ubo_light);
  glBindBuffer(GL_UNIFORM_BUFFER, light.ubo_light);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(ub_light_t), &ub_light, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, light.ubo_light); 
}

static bool shadow_init()
{
  glGenTextures(1, &shadow.depth_map);
  glBindTexture(GL_TEXTURE_2D, shadow.depth_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, SHADOWS_MAX * SHADOW_SIZE, SHADOW_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  glGenFramebuffers(1, &shadow.depth_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, shadow.depth_fbo);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow.depth_map, 0);
  glDrawBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  if (!shader_load(&shadow.shader, "shadow")) {
    return false;
  }
  
  GLuint ubl_matrices = glGetUniformBlockIndex(shadow.shader, "ubo_matrices");
  glUniformBlockBinding(shadow.shader, ubl_matrices, 0);
  
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
  
  light_update_point_shadow(id, pos);
}

void light_update_point_shadow(int id, vec3_t pos)
{
  vec3_t at[] = {
    vec3_init( 1.0,  0.0,  0.0),
    vec3_init(-1.0,  0.0,  0.0),
    vec3_init( 0.0,  1.0,  0.0),
    vec3_init( 0.0, -1.0,  0.0),
    vec3_init( 0.0,  0.0,  1.0),
    vec3_init( 0.0,  0.0, -1.0)
  };
  
  vec3_t up[] = {
    vec3_init(0.0, -1.0,  0.0),
    vec3_init(0.0, -1.0,  0.0),
    vec3_init(0.0,  0.0,  1.0),
    vec3_init(0.0,  0.0, -1.0),
    vec3_init(0.0, -1.0,  0.0),
    vec3_init(0.0, -1.0,  0.0)
  };
  
  glBindFramebuffer(GL_FRAMEBUFFER, shadow.depth_fbo);
  glUseProgram(shadow.shader);
  
  view_t view;
  view_set_viewport(&view, 0, 0, SHADOW_SIZE, SHADOW_SIZE);
  view_set_perspective(&view, to_radians(90.0), 0.1, 100.0);
  
  ub_shadow_t shadows[CUBE_FACES];
  
  for (int i = 0; i < CUBE_FACES; i++) {
    view_set_viewport(&view, (id * CUBE_FACES + i) * SHADOW_SIZE, 0, SHADOW_SIZE, SHADOW_SIZE);
    camera_set_view(view);
    camera_look_at(vec3_add(pos, at[i]), pos, up[i]);
    
    shadows[i].light_matrix = camera_get_mat_vp();
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    renderer_shadow_pass();
    glDisable(GL_CULL_FACE);
  }
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  glBindBuffer(GL_UNIFORM_BUFFER, light.ubo_light);
  glBufferSubData(GL_UNIFORM_BUFFER, offsetof(ub_light_t, shadows) + id * sizeof(shadows), sizeof(shadows), shadows);
}

void light_bind()
{
  glUseProgram(light.shader);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, shadow.depth_map);
}
