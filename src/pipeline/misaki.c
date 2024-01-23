#include "pipeline.h"

#include "../renderer/camera.h"
#include "../renderer/material.h"
#include "../renderer/skybox.h"
#include "../renderer/defer.h"
#include "../renderer/frame.h"
#include "../renderer/light.h"
#include "../renderer/ssao.h"
#include "../renderer/api.h"
#include "../gl/quad.h"
#include "../gl/gl.h"
#include "../gl/shader.h"

static bool misaki_init();
static void misaki_setup();
static void misaki_pass();

pipeline_t pipeline_misaki = {
  .init = misaki_init,
  .setup = misaki_setup,
  .pass = misaki_pass
};

static struct {
  GLuint shader;
  GLuint hdr;
  GLuint dither;
  frame_t frame_1;
  frame_t frame_2;
} misaki;

static bool misaki_init()
{
  frame_new(&misaki.frame_1, VIEW_WIDTH, VIEW_HEIGHT);
  frame_new(&misaki.frame_2, VIEW_WIDTH, VIEW_HEIGHT);
  
  shader_setup_t shader_setup;
  shader_setup_init(&shader_setup, "misaki");
  shader_setup_import(&shader_setup, SHADER_BOTH, "camera");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "light");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "ssao");
  
  if (!defer_shader_source(&shader_setup, "assets/pipeline/misaki", "defer")) {
    return false;
  }
  
  if (!shader_setup_compile(&misaki.shader, &shader_setup)) {
    return false;
  }
  
  ssao_shader_setup(misaki.shader);
  defer_shader_setup(misaki.shader);
  camera_shader_setup(misaki.shader);
  light_shader_setup(misaki.shader);
  light_shader_setup_shadow(misaki.shader);
  
  glUseProgram(misaki.shader);
  GLuint ul_skybox = glGetUniformLocation(misaki.shader, "u_skybox");
  glUniform1i(ul_skybox, TEXTURE_SKYBOX_BINDING);
  
  shader_setup_free(&shader_setup);
  
  if (!frame_shader_load(&misaki.hdr, "hdr")) {
    return false;
  }
  
  if (!frame_shader_load(&misaki.dither, "dither")) {
    return false;
  }
  
  return true;
}

static void misaki_setup()
{
  
}

static void misaki_pass()
{
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  
  defer_begin();
  renderer_scene_pass();
  defer_end();
  
  frame_begin(misaki.frame_1);
  glActiveTexture(GL_TEXTURE0 + TEXTURE_SKYBOX_BINDING);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_get_texture());
  light_bind_depth_map(misaki.shader);
  defer_draw(misaki.shader);
  frame_end();
  
  frame_begin(misaki.frame_2);
  frame_draw(misaki.dither, misaki.frame_1);
  frame_end();
  
  camera_set_viewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  frame_draw(misaki.hdr, misaki.frame_2);
}
