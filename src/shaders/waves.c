#include "waves.h"

#include "../common/log.h"
#include "../common/file.h"

#define WAVES_SIZE    1024
#define NUM_WAVES     10
#define PATTERN_SIZE  64
#define BORDER_SIZE   2

static void waves_render_waves_map(waves_t *waves);
static void waves_copy(waves_t *waves, full_bright_t *full_bright, view_t *view);
static void waves_render_normal_map(waves_t *waves);
static void waves_init_pattern(waves_t *waves);
static void waves_init_maps(waves_t *waves);
static bool waves_init_shaders(waves_t *waves);

bool waves_init(waves_t *waves, mesh_t quad_mesh)
{
  if (!waves_init_shaders(waves))
    return false;
  
  waves_init_pattern(waves);
  waves_init_maps(waves);
  
  waves->normal_map = waves->wave[0];
  waves->quad_mesh = quad_mesh;
  
  return true;
}

static void waves_init_pattern(waves_t *waves)
{
  float data[PATTERN_SIZE * PATTERN_SIZE * 3];
  
  int n = PATTERN_SIZE / 2;
  
  for (int i = 0; i < PATTERN_SIZE; i++) {
    for (int j = 0; j < PATTERN_SIZE; j++) {
      float x = i - (float) n;
      float y = j - (float) n;
      
      float t = sqrt(x*x + y*y);
      
      if (t > n) {
        data[(i * PATTERN_SIZE + j) * 3 + 0] = 0.5;
        data[(i * PATTERN_SIZE + j) * 3 + 1] = 0.5;
        data[(i * PATTERN_SIZE + j) * 3 + 2] = 0.5;
      } else {
        float theta = t / n * M_PI;
        
        float u = sin(theta) * 0.2;
        float u_t = cos(theta) * 0.2 / n * 49 * sqrt(4.0);
        float k = 0.015;
        
        data[(i * PATTERN_SIZE + j) * 3 + 0] = u * 0.995 + 0.5;
        data[(i * PATTERN_SIZE + j) * 3 + 1] = u + u_t * k + 0.5;
        data[(i * PATTERN_SIZE + j) * 3 + 2] = 0.5;
      }
    }
  }
  
  glGenTextures(1, &waves->pattern);
  glBindTexture(GL_TEXTURE_2D, waves->pattern);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, PATTERN_SIZE, PATTERN_SIZE, 0, GL_RGB, GL_FLOAT, data);
}

static void waves_init_maps(waves_t *waves)
{
  for (int i = 0; i < 2; i++) {
    glGenFramebuffers(1, &waves->fbo[i]);
    glBindFramebuffer(GL_FRAMEBUFFER, waves->fbo[i]);
    
    glGenTextures(1, &waves->wave[i]);
    
    glBindTexture(GL_TEXTURE_2D, waves->wave[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WAVES_SIZE, WAVES_SIZE, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, waves->wave[i], 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
}

static bool waves_init_shaders(waves_t *waves)
{
  char *src_vertex = file_read_all("res/shader/hdr.vert");
  char *src_fragment = file_read_all("res/shader/waves.frag");

  if (!shader_load(&waves->shader, "", src_vertex, src_fragment)) {
    LOG_ERROR("failed to load shader");
    return false;
  }
  
  char *src_out = file_read_all("res/shader/waves_out.frag");
  
  if (!shader_load(&waves->out_shader, "", src_vertex, src_out)) {
    LOG_ERROR("failed to load shader");
    return false;
  }
  
  free(src_vertex);
  free(src_fragment);
  free(src_out);
  
  return true;
}

void waves_move(waves_t *waves, full_bright_t *full_bright, view_t *view)
{
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  
  glViewport(0, 0, WAVES_SIZE, WAVES_SIZE);
  
  glScissor(BORDER_SIZE, BORDER_SIZE, WAVES_SIZE - BORDER_SIZE * 2, WAVES_SIZE - BORDER_SIZE * 2);
  glEnable(GL_SCISSOR_TEST);
  waves_render_waves_map(waves);
  waves_copy(waves, full_bright, view);
  glDisable(GL_SCISSOR_TEST);
  
  waves_render_normal_map(waves);
  
  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
}

static void waves_render_waves_map(waves_t *waves)
{
  glBindFramebuffer(GL_FRAMEBUFFER, waves->fbo[0]);
  
  glUseProgram(waves->shader);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, waves->wave[1]);
  draw_mesh(waves->quad_mesh);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void waves_copy(waves_t *waves, full_bright_t *full_bright, view_t *view)
{
  glBindFramebuffer(GL_FRAMEBUFFER, waves->fbo[1]);
  
  full_bright_bind(full_bright);
  
  view_set(view, mat4x4_init_identity(), vec3_init(0.0, 0.0, 0.0));
  view_sub_data(view, mat4x4_init_identity());
  
  material_t material = { .diffuse = waves->wave[0] };
  full_bright_set_material(&material);
  draw_mesh(waves->quad_mesh);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void waves_render_normal_map(waves_t *waves)
{
  glBindFramebuffer(GL_FRAMEBUFFER, waves->fbo[0]);
  
  glUseProgram(waves->out_shader);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, waves->wave[1]);
  draw_mesh(waves->quad_mesh);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void waves_setup(waves_t *waves, full_bright_t *full_bright, view_t *view)
{
  glBindFramebuffer(GL_FRAMEBUFFER, waves->fbo[1]);
  
  glDisable(GL_BLEND);
  
  glViewport(0, 0, WAVES_SIZE, WAVES_SIZE);
  glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glEnable(GL_SCISSOR_TEST);
  glScissor(2, 2, WAVES_SIZE - 4, WAVES_SIZE - 4);
  
  full_bright_bind(full_bright);
  view_set(view, mat4x4_init_identity(), vec3_init(0.0, 0.0, 0.0));
  
  material_t material = { .diffuse = waves->pattern };
  
  for (int i = 0; i < NUM_WAVES; i++) {
    float x = (rand() % 256) / 128.0 - 1.0;
    float y = (rand() % 256) / 128.0 - 1.0;
    float t = (float) PATTERN_SIZE / (float) WAVES_SIZE;// 0.2 + (rand() % 256) / 256.0 * 0.2;
    
    view_sub_data(
      view,
      mat4x4_init_transform(
        vec3_init(x, y, 1.0f),
        vec3_init(t, t, 1.0f)
      )
    );
  
    full_bright_set_material(&material);
    draw_mesh(waves->quad_mesh);
  }
  
  glDisable(GL_SCISSOR_TEST);
  glEnable(GL_BLEND);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void waves_show(waves_t *waves, full_bright_t *full_bright, view_t *view)
{
  glViewport(50, 50, 600, 600);
  
  glDisable(GL_DEPTH_TEST);
  
  full_bright_bind(full_bright);
  
  view_set(view, mat4x4_init_identity(), vec3_init(0.0, 0.0, 0.0));
  view_sub_data(view, mat4x4_init_identity());
  
  material_t material = { .diffuse = waves->normal_map };
  full_bright_set_material(&material);
  draw_mesh(waves->quad_mesh);
  
  glEnable(GL_DEPTH_TEST);
}
