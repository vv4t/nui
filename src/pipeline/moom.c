#include "pipeline.h"

#include "../renderer/camera.h"
#include "../renderer/model.h"
#include "../renderer/material.h"
#include "../renderer/wave.h"
#include "../renderer/defer.h"
#include "../renderer/frame.h"
#include "../renderer/ssao.h"
#include "../renderer/light.h"
#include "../renderer/skybox.h"
#include "../renderer/api.h"
#include "../gl/quad.h"
#include "../gl/gl.h"
#include "../gl/shader.h"

static bool moom_init();
static void moom_setup();
static void moom_pass();
static void moom_draw_water();

pipeline_t pipeline_moom = {
  .init = moom_init,
  .setup = moom_setup,
  .pass = moom_pass
};

static struct {
  GLuint shader;
  GLuint forward;
  GLuint post;
  GLuint hdr;
  GLuint dither;
  
  frame_t frame_1;
  frame_t frame_2;
  
  wave_t wave;
  model_t model;
} moom;

static bool moom_defer_setup();
static bool moom_post_setup();
static bool moom_forward_setup();

static bool moom_init()
{
  frame_new(&moom.frame_1, VIEW_WIDTH, VIEW_HEIGHT);
  frame_new(&moom.frame_2, VIEW_WIDTH, VIEW_HEIGHT);
  
  if (!moom_defer_setup()) {
    return false;
  }
  
  if (!moom_post_setup()) {
    return false;
  }
  
  if (!moom_forward_setup()) {
    return false;
  }
  
  if (!frame_shader_load(&moom.hdr, "hdr")) {
    return false;
  }
  
  if (!frame_shader_load(&moom.dither, "dither")) {
    return false;
  }
  
  if (!model_load(&moom.model, "pool")) {
    return false;
  }
  
  wave_new(&moom.wave, 400, 400);
  moom.model.subgroups[0].material.normal = moom.wave.normal;
  
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
  
  return true;
}

static bool moom_defer_setup()
{
  shader_setup_t shader_setup;
  shader_setup_init(&shader_setup, "moom");
  shader_setup_import(&shader_setup, SHADER_BOTH, "camera");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "light");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "ssao");
  
  if (!defer_shader_source(&shader_setup, "assets/pipeline/moom", "defer")) {
    return false;
  }
  
  if (!shader_setup_compile(&moom.shader, &shader_setup)) {
    return false;
  }
  
  ssao_shader_setup(moom.shader);
  defer_shader_setup(moom.shader);
  camera_shader_setup(moom.shader);
  light_shader_setup(moom.shader);
  light_shader_setup_shadow(moom.shader);
  
  shader_setup_free(&shader_setup);
  
  return true;
}

static bool moom_post_setup()
{
  shader_setup_t shader_setup;
  shader_setup_init(&shader_setup, "post");
  shader_setup_import(&shader_setup, SHADER_BOTH, "camera");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "light");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "ssao");
  
  if (!defer_shader_source(&shader_setup, "assets/pipeline/moom", "post")) {
    return false;
  }
  
  if (!shader_setup_compile(&moom.post, &shader_setup)) {
    return false;
  }
  
  ssao_shader_setup(moom.post);
  defer_shader_setup(moom.post);
  camera_shader_setup(moom.post);
  light_shader_setup(moom.post);
  light_shader_setup_shadow(moom.post);
  
  shader_setup_free(&shader_setup);
  
  return true;
}

static bool moom_forward_setup()
{
  shader_setup_t shader_setup;
  shader_setup_init(&shader_setup, "forward");
  shader_setup_import(&shader_setup, SHADER_BOTH, "camera");
  shader_setup_import(&shader_setup, SHADER_BOTH, "material");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "ssr");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "light");
  
  if (!shader_setup_source(&shader_setup, "assets/pipeline/moom", "forward")) {
    return false;
  }
  
  if (!shader_setup_compile(&moom.forward, &shader_setup)) {
    return false;
  }
  
  camera_shader_setup(moom.forward);
  material_shader_setup(moom.forward);
  light_shader_setup(moom.forward);
  light_shader_setup_shadow(moom.forward);
  
  GLuint ul_pos = glGetUniformLocation(moom.forward, "d_pos");
  GLuint ul_normal = glGetUniformLocation(moom.forward, "d_normal");
  GLuint ul_albedo = glGetUniformLocation(moom.forward, "d_albedo");
  
  glUniform1i(ul_pos, G_BUFFER_POS_BINDING);
  glUniform1i(ul_normal, G_BUFFER_NORMAL_BINDING);
  glUniform1i(ul_albedo, G_BUFFER_ALBEDO_BINDING);
  
  shader_setup_free(&shader_setup);
  
  return true;
}

static void moom_setup()
{
  wave_update(&moom.wave);
}

static void moom_pass()
{
  defer_begin();
  renderer_scene_pass();
  defer_end();
  
  frame_begin(moom.frame_1);
  light_bind_depth_map(moom.shader);
  defer_draw(moom.shader);
  frame_end();
  
  frame_begin(moom.frame_2);
  frame_draw(moom.dither, moom.frame_1);
  fbo_blit(moom.frame_2.fbo, defer_get_fbo(), GL_DEPTH_BUFFER_BIT);
  moom_draw_water();
  frame_end();
  
  frame_begin(moom.frame_1);
  glUseProgram(moom.post);
  defer_bind();
  glActiveTexture(GL_TEXTURE0 + G_BUFFER_ALBEDO_BINDING);
  glBindTexture(GL_TEXTURE_2D, moom.frame_2.texture);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  quad_draw();
  frame_end();
  
  camera_set_viewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  frame_draw(moom.hdr, moom.frame_1);
}

static void moom_draw_water()
{
  glUseProgram(moom.forward);
  glEnable(GL_BLEND);
  
  glActiveTexture(GL_TEXTURE0 + G_BUFFER_ALBEDO_BINDING);
  glBindTexture(GL_TEXTURE_2D, moom.frame_1.texture);
  
  light_bind_depth_map(moom.forward);
  camera_model(mat4x4_init_identity());
  model_draw(&moom.model);
  glDisable(GL_BLEND);
}
