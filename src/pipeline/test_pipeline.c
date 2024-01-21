#include "test_pipeline.h"

#include "../renderer/skybox.h"
#include "../renderer/camera.h"
#include "../renderer/material.h"
#include "../renderer/defer.h"
#include "../renderer/frame.h"
#include "../renderer/light.h"
#include "../renderer/ssao.h"
#include "../renderer/wave.h"
#include "../renderer/api.h"
#include "../renderer/model.h"
#include "../renderer/cubemap.h"
#include "../gl/quad.h"
#include "../gl/gl.h"
#include "../gl/shader.h"

struct {
  GLuint defer_shader;
  GLuint forward_shader;
  GLuint hdr;
  GLuint dither;
  wave_t wave;
  model_t model;
} test_pipeline;

static bool test_pipeline_defer_init();
static bool test_pipeline_forward_init();

bool test_pipeline_init()
{
  if (!wave_init()) {
    return false;
  }
  
  if (!skybox_init("night")) {
    return false;
  }
  
  if (!cubemap_init()) {
    return false;
  }
  
  if (!test_pipeline_defer_init()) {
    return false;
  }
  
  if (!test_pipeline_forward_init()) {
    return false;
  }
  
  if (!frame_shader_load(&test_pipeline.hdr, "hdr")) {
    return false;
  }
  
  if (!frame_shader_load(&test_pipeline.dither, "dither")) {
    return false;
  }
  
  if (!model_load(&test_pipeline.model, "pool")) {
    return false;
  }
  
  wave_new(&test_pipeline.wave, 1024, 1024);
  
  test_pipeline.model.subgroups[0].material.normal = test_pipeline.wave.normal;
  
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
  
  return true;
}

bool test_pipeline_defer_init()
{
  shader_setup_t shader_setup;
  shader_setup_init(&shader_setup, "defer_shader");
  shader_setup_import(&shader_setup, SHADER_BOTH, "camera");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "light");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "ssao");
  defer_shader_source(&shader_setup, "light");
  
  if (!shader_setup_compile(&test_pipeline.defer_shader, &shader_setup)) {
    return false;
  }
  
  ssao_shader_setup(test_pipeline.defer_shader);
  defer_shader_setup(test_pipeline.defer_shader);
  camera_shader_setup(test_pipeline.defer_shader);
  light_shader_setup(test_pipeline.defer_shader);
  light_shader_setup_shadow(test_pipeline.defer_shader);
  
  glUseProgram(test_pipeline.defer_shader);
  
  GLuint ul_cubemap = glGetUniformLocation(test_pipeline.defer_shader, "u_cubemap");
  glUniform1i(ul_cubemap, TEXTURE_CUBEMAP_BINDING);
  
  GLuint ul_skybox = glGetUniformLocation(test_pipeline.defer_shader, "u_skybox");
  glUniform1i(ul_skybox, TEXTURE_SKYBOX_BINDING);
  
  shader_setup_free(&shader_setup);
  
  return true;
}

bool test_pipeline_forward_init()
{
  shader_setup_t forward_shader_setup;
  shader_setup_init(&forward_shader_setup, "forward_shader");
  shader_setup_import(&forward_shader_setup, SHADER_BOTH, "camera");
  shader_setup_import(&forward_shader_setup, SHADER_BOTH, "material");
  shader_setup_import(&forward_shader_setup, SHADER_FRAGMENT, "light");
  shader_setup_source(&forward_shader_setup, "light");
  
  if (!shader_setup_compile(&test_pipeline.forward_shader, &forward_shader_setup)) {
    return false;
  }
  
  camera_shader_setup(test_pipeline.forward_shader);
  material_shader_setup(test_pipeline.forward_shader);
  light_shader_setup(test_pipeline.forward_shader);
  light_shader_setup_shadow(test_pipeline.forward_shader);
  
  glUseProgram(test_pipeline.forward_shader);
  
  GLuint ul_cubemap = glGetUniformLocation(test_pipeline.forward_shader, "u_cubemap");
  glUniform1i(ul_cubemap, TEXTURE_CUBEMAP_BINDING);
  
  shader_setup_free(&forward_shader_setup);
  
  return true;
}

void test_pipeline_pass()
{
  wave_update(&test_pipeline.wave);
  
  defer_begin();
  renderer_scene_pass();
  defer_end();
  
  frame_begin(0);
  
  glActiveTexture(GL_TEXTURE0 + TEXTURE_SKYBOX_BINDING);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_get_texture());
  
  glActiveTexture(GL_TEXTURE0 + TEXTURE_CUBEMAP_BINDING);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_get_texture());
  
  light_bind_depth_map(test_pipeline.defer_shader);
  defer_draw(test_pipeline.defer_shader);
  
  glBindFramebuffer(GL_READ_FRAMEBUFFER, defer_get_fbo());
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_get_fbo(0));
  glBlitFramebuffer(0, 0, VIEW_WIDTH, VIEW_HEIGHT, 0, 0, VIEW_WIDTH, VIEW_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  
  glEnable(GL_BLEND);
  
  glUseProgram(test_pipeline.forward_shader);
  glActiveTexture(GL_TEXTURE0 + TEXTURE_CUBEMAP_BINDING);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_get_texture());
  light_bind_depth_map(test_pipeline.forward_shader);
  camera_model(mat4x4_init_identity());
  model_draw(&test_pipeline.model);
  
  glDisable(GL_BLEND);
  
  frame_end();
  
  frame_begin(1);
  frame_draw(test_pipeline.dither, 0);
  frame_end();
  
  camera_set_viewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  frame_draw(test_pipeline.hdr, 1);
}
