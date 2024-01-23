#include "pipeline.h"

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
#include "../ngui/ngui.h"
#include "../gl/quad.h"
#include "../gl/gl.h"
#include "../gl/shader.h"

static bool chise_init();
static void chise_setup();
static void chise_pass();

pipeline_t pipeline_chise = {
  .init = chise_init,
  .setup = chise_setup,
  .pass = chise_pass
};

static struct {
  GLuint shader;
  GLuint forward_shader;
  GLuint hdr;
  GLuint dither;
  wave_t wave;
  model_t model;
} chise;

static bool chise_init()
{
  if (!wave_init()) {
    return false;
  }
  
  if (!skybox_init("night")) {
    return false;
  }
  
  shader_setup_t shader_setup;
  shader_setup_init(&shader_setup, "defer_shader");
  shader_setup_import(&shader_setup, SHADER_BOTH, "camera");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "light");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "ssao");
  
  if (!defer_shader_source(&shader_setup, "assets/pipeline/chise", "defer")) {
    return false;
  }
  
  if (!shader_setup_compile(&chise.shader, &shader_setup)) {
    return false;
  }
  
  ssao_shader_setup(chise.shader);
  defer_shader_setup(chise.shader);
  camera_shader_setup(chise.shader);
  light_shader_setup(chise.shader);
  light_shader_setup_shadow(chise.shader);
  
  glUseProgram(chise.shader);
  GLuint ul_skybox = glGetUniformLocation(chise.shader, "u_skybox");
  glUniform1i(ul_skybox, TEXTURE_SKYBOX_BINDING);
  
  shader_setup_free(&shader_setup);
  
  shader_setup_t forward_shader_setup;
  shader_setup_init(&forward_shader_setup, "forward_shader");
  shader_setup_import(&forward_shader_setup, SHADER_BOTH, "camera");
  shader_setup_import(&forward_shader_setup, SHADER_BOTH, "material");
  shader_setup_import(&forward_shader_setup, SHADER_FRAGMENT, "light");
  
  if (!shader_setup_source(&forward_shader_setup, "assets/pipeline/akariin", "forward")) {
    return false;
  }
  
  if (!shader_setup_compile(&chise.forward_shader, &forward_shader_setup)) {
    return false;
  }
  
  camera_shader_setup(chise.forward_shader);
  material_shader_setup(chise.forward_shader);
  light_shader_setup(chise.forward_shader);
  light_shader_setup_shadow(chise.forward_shader);
  
  glUseProgram(chise.forward_shader);
  ul_skybox = glGetUniformLocation(chise.forward_shader, "u_skybox");
  glUniform1i(ul_skybox, TEXTURE_SKYBOX_BINDING);
  
  shader_setup_free(&forward_shader_setup);
  
  if (!frame_shader_load(&chise.hdr, "hdr")) {
    return false;
  }
  
  if (!frame_shader_load(&chise.dither, "dither")) {
    return false;
  }
  
  if (!model_load(&chise.model, "pool")) {
    return false;
  }
  
  wave_new(&chise.wave, 400, 400);
  
  chise.model.subgroups[0].material.normal = chise.wave.normal;
  
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
  
  return true;
}

static void chise_setup()
{
  wave_update(&chise.wave);
}

static void chise_pass()
{
  defer_begin();
  renderer_scene_pass();
  defer_end();
  
  frame_begin(0);
  
  glActiveTexture(GL_TEXTURE0 + TEXTURE_SKYBOX_BINDING);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_get_texture());
  light_bind_depth_map(chise.shader);
  defer_draw(chise.shader);
  
  glBindFramebuffer(GL_READ_FRAMEBUFFER, defer_get_fbo());
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_get_fbo(0));
  glBlitFramebuffer(0, 0, VIEW_WIDTH, VIEW_HEIGHT, 0, 0, VIEW_WIDTH, VIEW_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  
  glEnable(GL_BLEND);
  
  glUseProgram(chise.forward_shader);
  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_get_texture());
  light_bind_depth_map(chise.forward_shader);
  camera_model(mat4x4_init_identity());
  model_draw(&chise.model);
  
  glDisable(GL_BLEND);
  
  frame_end();
  
  frame_begin(1);
  frame_draw(chise.dither, 0);
  frame_end();
  
  glEnable(GL_BLEND);
  
  camera_set_viewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  frame_draw(chise.hdr, 1);
  
  ngui_render();
  
  glDisable(GL_BLEND);
}
