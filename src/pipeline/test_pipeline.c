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
#include "../gl/quad.h"
#include "../gl/gl.h"
#include "../gl/shader.h"

struct {
  GLuint shader;
  GLuint forward_shader;
  GLuint hdr;
  GLuint dither;
  wave_t wave;
  model_t model;
} test_pipeline;

bool test_pipeline_init()
{
  if (!wave_init()) {
    return false;
  }
  
  if (!skybox_init("stars")) {
    return false;
  }
  
  shader_setup_t shader_setup;
  shader_setup_init(&shader_setup, "test_pipeline");
  shader_setup_import(&shader_setup, SHADER_BOTH, "camera");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "light");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "ssao");
  defer_shader_source(&shader_setup, "light");
  
  if (!shader_setup_compile(&test_pipeline.shader, &shader_setup)) {
    return false;
  }
  
  ssao_shader_setup(test_pipeline.shader);
  defer_shader_setup(test_pipeline.shader);
  camera_shader_setup(test_pipeline.shader);
  light_shader_setup(test_pipeline.shader);
  light_shader_setup_shadow(test_pipeline.shader);
  
  glUseProgram(test_pipeline.shader);
  GLuint ul_skybox = glGetUniformLocation(test_pipeline.shader, "u_skybox");
  glUniform1i(ul_skybox, 5);
  
  shader_setup_free(&shader_setup);
  
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
  ul_skybox = glGetUniformLocation(test_pipeline.forward_shader, "u_skybox");
  glUniform1i(ul_skybox, 5);
  
  shader_setup_free(&forward_shader_setup);
  
  if (!frame_shader_load(&test_pipeline.hdr, "hdr")) {
    return false;
  }
  
  if (!frame_shader_load(&test_pipeline.dither, "dither")) {
    return false;
  }
  
  if (!model_load(&test_pipeline.model, "pool")) {
    return false;
  }
  
  wave_new(&test_pipeline.wave, 400, 400);
  
  test_pipeline.model.subgroups[0].material.normal = test_pipeline.wave.normal;
  
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
  
  return true;
}

void test_pipeline_pass()
{
  wave_update(&test_pipeline.wave);
  
  defer_begin();
  renderer_scene_pass();
  defer_end();
  
  frame_begin(0);
  
  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_get_texture());
  light_bind_depth_map(test_pipeline.shader);
  defer_draw(test_pipeline.shader);
  
  glBindFramebuffer(GL_READ_FRAMEBUFFER, defer_get_fbo());
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_get_fbo(0));
  glBlitFramebuffer(0, 0, VIEW_WIDTH, VIEW_HEIGHT, 0, 0, VIEW_WIDTH, VIEW_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  
  glEnable(GL_BLEND);
  
  glUseProgram(test_pipeline.forward_shader);
  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_get_texture());
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
