#include "light.h"

#define CUBE_FACES 6
#define POINTS_MAX 8
#define SHADOW_SIZE 512

#include "camera.h"
#include "api.h"
#include "../gl/gl.h"
#include "../gl/shader.h"
#include "../common/log.h"

typedef struct {
  GLuint ubo_light;
} light_t;

typedef struct {
  GLuint shader;
  GLuint depth_map;
  GLuint depth_fbo;
} shadow_t;

static shadow_t shadow;
static light_t light;

typedef struct {
  vec3_t pos;
  float intensity;
  vec4_t color;
} ub_point_t;

typedef struct {
  ub_point_t points[POINTS_MAX];
} ub_light_t;

static bool shadow_init();

static void light_init_uniform_location();
static void light_init_ambient();
static void light_update_point_shadow(int id, vec3_t pos);

bool light_init()
{
  ub_light_t ub_light = {0};
  
  glGenBuffers(1, &light.ubo_light);
  glBindBuffer(GL_UNIFORM_BUFFER, light.ubo_light);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(ub_light_t), &ub_light, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, UBO_LIGHT_BINDING, light.ubo_light); 
  
  if (!shadow_init()) {
    return false;
  }
  
  return true;
}

void light_shader_setup(GLuint shader)
{
  glUseProgram(shader);
  
  GLuint ubl_light = glGetUniformBlockIndex(shader, "ub_light");
  glUniformBlockBinding(shader, ubl_light, UBO_LIGHT_BINDING);
}

void light_shader_setup_shadow(GLuint shader)
{
  glUseProgram(shader);
  
  GLuint ul_depth_map = glGetUniformLocation(shader, "u_depth_map");
  glUniform1i(ul_depth_map, TEXTURE_DEPTH_BINDING);
}

void light_sub_point(int id, vec3_t pos, float intensity, vec3_t color)
{
  ub_point_t point = {
    .pos = pos,
    .intensity = intensity,
    .color = vec4_init(color.x, color.y, color.z, 1.0)
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
  view_set_perspective(&view, 1.0, to_radians(90.0), 0.1, 100.0);
  
  camera_set_viewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  
  for (int i = 0; i < CUBE_FACES; i++) {
    camera_set_viewport(i * SHADOW_SIZE, id * SHADOW_SIZE, SHADOW_SIZE, SHADOW_SIZE);
    
    camera_set_view(view);
    camera_look_at(vec3_add(pos, at[i]), pos, up[i]);
    
    renderer_shadow_pass();
  }
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void light_bind_depth_map(GLuint shader)
{
  glUseProgram(shader);
  glActiveTexture(GL_TEXTURE0 + TEXTURE_DEPTH_BINDING);
  glBindTexture(GL_TEXTURE_2D, shadow.depth_map);
}

static bool shadow_init()
{
  glGenFramebuffers(1, &shadow.depth_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, shadow.depth_fbo);
  
  glGenTextures(1, &shadow.depth_map);
  glBindTexture(GL_TEXTURE_2D, shadow.depth_map);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, CUBE_FACES * SHADOW_SIZE, POINTS_MAX * SHADOW_SIZE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow.depth_map, 0);
  
  // glDrawBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  shader_setup_t shader_setup;
  shader_setup_init(&shader_setup, "shadow");
  shader_setup_import(&shader_setup, SHADER_BOTH, "camera");
  
  if (!shader_setup_source(&shader_setup, "assets/shader/shadow", "shadow")) {
    return false;
  }
  
  if (!shader_setup_compile(&shadow.shader, &shader_setup)) {
    return false;
  }
  
  camera_shader_setup(shadow.shader);
  
  shader_setup_free(&shader_setup);
  
  return true;
}
