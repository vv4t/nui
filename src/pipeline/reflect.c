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

static bool reflect_init();
static void reflect_setup();
static void reflect_pass();

pipeline_t pipeline_reflect = {
  .init = reflect_init,
  .setup = reflect_setup,
  .pass = reflect_pass
};

static struct {
  GLuint shader;
  GLuint forward_shader;
  GLuint hdr;
  GLuint dither;
  frame_t frame_1;
  frame_t frame_2;
  wave_t wave;
  model_t model;
} reflect;

static bool reflect_defer_setup();
static bool reflect_forward_setup();

static bool reflect_init()
{
  if (!skybox_init("night2")) {
    return false;
  }
  
  frame_new(&reflect.frame_1, VIEW_WIDTH, VIEW_HEIGHT);
  frame_new(&reflect.frame_2, VIEW_WIDTH, VIEW_HEIGHT);
  
  if (!reflect_defer_setup()) {
    return false;
  }
  
  if (!reflect_forward_setup()) {
    return false;
  }
  
  if (!frame_shader_load(&reflect.hdr, "hdr")) {
    return false;
  }
  
  if (!frame_shader_load(&reflect.dither, "dither")) {
    return false;
  }
  
  if (!model_load(&reflect.model, "pool")) {
    return false;
  }
  
  wave_new(&reflect.wave, 400, 400);
  reflect.model.subgroups[0].material.normal = reflect.wave.normal;
  
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
  
  return true;
}

static bool reflect_defer_setup()
{
  shader_setup_t shader_setup;
  shader_setup_init(&shader_setup, "reflect");
  shader_setup_import(&shader_setup, SHADER_BOTH, "camera");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "light");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "ssao");
  
  if (!defer_shader_source(&shader_setup, "assets/pipeline/reflect", "defer")) {
    return false;
  }
  
  if (!shader_setup_compile(&reflect.shader, &shader_setup)) {
    return false;
  }
  
  ssao_shader_setup(reflect.shader);
  defer_shader_setup(reflect.shader);
  camera_shader_setup(reflect.shader);
  light_shader_setup(reflect.shader);
  light_shader_setup_shadow(reflect.shader);
  
  glUseProgram(reflect.shader);
  GLuint ul_skybox = glGetUniformLocation(reflect.shader, "u_skybox");
  glUniform1i(ul_skybox, TEXTURE_SKYBOX_BINDING);
  
  shader_setup_free(&shader_setup);
  
  return true;
}

static bool reflect_forward_setup()
{
  shader_setup_t shader_setup;
  shader_setup_init(&shader_setup, "forward_shader");
  shader_setup_import(&shader_setup, SHADER_BOTH, "camera");
  shader_setup_import(&shader_setup, SHADER_BOTH, "material");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "ssr");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "light");
  
  if (!shader_setup_source(&shader_setup, "assets/pipeline/reflect", "forward")) {
    return false;
  }
  
  if (!shader_setup_compile(&reflect.forward_shader, &shader_setup)) {
    return false;
  }
  
  camera_shader_setup(reflect.forward_shader);
  material_shader_setup(reflect.forward_shader);
  light_shader_setup(reflect.forward_shader);
  light_shader_setup_shadow(reflect.forward_shader);
  
  GLuint ul_pos = glGetUniformLocation(reflect.forward_shader, "d_pos");
  GLuint ul_normal = glGetUniformLocation(reflect.forward_shader, "d_normal");
  GLuint ul_albedo = glGetUniformLocation(reflect.forward_shader, "d_albedo");
  
  glUniform1i(ul_pos, G_BUFFER_POS_BINDING);
  glUniform1i(ul_normal, G_BUFFER_NORMAL_BINDING);
  glUniform1i(ul_albedo, G_BUFFER_ALBEDO_BINDING);
  
  glUseProgram(reflect.forward_shader);
  GLuint ul_skybox = glGetUniformLocation(reflect.forward_shader, "u_skybox");
  glUniform1i(ul_skybox, TEXTURE_SKYBOX_BINDING);
  
  shader_setup_free(&shader_setup);
  
  return true;
}

static void reflect_setup()
{
  wave_update(&reflect.wave);
}

static void reflect_pass()
{
  defer_begin();
  renderer_scene_pass();
  defer_end();
  
  frame_begin(reflect.frame_1);
  
  glActiveTexture(GL_TEXTURE0 + TEXTURE_SKYBOX_BINDING);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_get_texture());
  light_bind_depth_map(reflect.shader);
  defer_draw(reflect.shader);
  
  frame_end();
  
  frame_begin(reflect.frame_2);
  frame_draw(reflect.dither, reflect.frame_1);
  
  glBindFramebuffer(GL_READ_FRAMEBUFFER, defer_get_fbo());
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, reflect.frame_2.fbo);
  glBlitFramebuffer(0, 0, VIEW_WIDTH, VIEW_HEIGHT, 0, 0, VIEW_WIDTH, VIEW_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  
  glEnable(GL_BLEND);
  
  glUseProgram(reflect.forward_shader);
  
  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_get_texture());
  
  glActiveTexture(GL_TEXTURE0 + G_BUFFER_ALBEDO_BINDING);
  glBindTexture(GL_TEXTURE_2D, reflect.frame_1.texture);
  
  light_bind_depth_map(reflect.forward_shader);
  camera_model(mat4x4_init_identity());
  model_draw(&reflect.model);
  
  glDisable(GL_BLEND);
  
  frame_end();
  
  camera_set_viewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  frame_draw(reflect.hdr, reflect.frame_2);
}
