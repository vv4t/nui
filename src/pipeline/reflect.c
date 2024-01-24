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
  GLuint hdr;
  GLuint dither;
  frame_t frame_1;
  frame_t frame_2;
  wave_t wave;
  model_t model;
} reflect;

static bool reflect_init()
{
  if (!skybox_init("night")) {
    return false;
  }
  
  frame_new(&reflect.frame_1, VIEW_WIDTH, VIEW_HEIGHT);
  frame_new(&reflect.frame_2, VIEW_WIDTH, VIEW_HEIGHT);
  
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
  
  return true;
}

static void reflect_setup()
{
  wave_update(&reflect.wave);
}

static void reflect_pass()
{
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  
  defer_begin();
  renderer_scene_pass();
  model_draw(&reflect.model);
  defer_end();
  
  frame_begin(reflect.frame_1);
  glActiveTexture(GL_TEXTURE0 + TEXTURE_SKYBOX_BINDING);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_get_texture());
  light_bind_depth_map(reflect.shader);
  defer_draw(reflect.shader);
  frame_end();
  
  frame_begin(reflect.frame_2);
  frame_draw(reflect.dither, reflect.frame_1);
  frame_end();
  
  camera_set_viewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  frame_draw(reflect.hdr, reflect.frame_2);
}
