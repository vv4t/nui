#include "pipeline.h"

#include "../renderer/camera.h"
#include "../renderer/material.h"
#include "../renderer/defer.h"
#include "../renderer/frame.h"
#include "../renderer/light.h"
#include "../renderer/api.h"
#include "../renderer/skybox.h"
#include "../gl/quad.h"
#include "../gl/gl.h"
#include "../gl/shader.h"

static bool akariin_init();
static void akariin_setup();
static void akariin_pass();

pipeline_t pipeline_akariin = {
  .init = akariin_init,
  .setup = akariin_setup,
  .pass = akariin_pass
};

static struct {
  GLuint shader;
  GLuint hdr;
  GLuint dither;
  frame_t frame_1;
  frame_t frame_2;
} akariin;

static bool akariin_init()
{
  if (!skybox_init("night")) {
    return false;
  }
  
  frame_new(&akariin.frame_1, VIEW_WIDTH, VIEW_HEIGHT);
  frame_new(&akariin.frame_2, VIEW_WIDTH, VIEW_HEIGHT);
  
  shader_setup_t shader_setup;
  shader_setup_init(&shader_setup, "akariin");
  shader_setup_import(&shader_setup, SHADER_BOTH, "camera");
  shader_setup_import(&shader_setup, SHADER_BOTH, "material");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "light");
  
  if (!shader_setup_source(&shader_setup, "assets/pipeline/akariin", "forward")) {
    return false;
  }
  
  if (!shader_setup_compile(&akariin.shader, &shader_setup)) {
    return false;
  }
  
  camera_shader_setup(akariin.shader);
  material_shader_setup(akariin.shader);
  light_shader_setup(akariin.shader);
  light_shader_setup_shadow(akariin.shader);
  
  glUseProgram(akariin.shader);
  GLuint ul_skybox = glGetUniformLocation(akariin.shader, "u_skybox");
  glUniform1i(ul_skybox, TEXTURE_SKYBOX_BINDING);
  
  shader_setup_free(&shader_setup);
  
  if (!frame_shader_load(&akariin.hdr, "hdr")) {
    return false;
  }
  
  if (!frame_shader_load(&akariin.dither, "dither")) {
    return false;
  }
  
  return true;
}

static void akariin_setup()
{
  
}

static void akariin_pass()
{
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  
  frame_begin(akariin.frame_1);
  skybox_render();
  light_bind_depth_map(akariin.shader);
  renderer_scene_pass();
  frame_end();
  
  frame_begin(akariin.frame_2);
  frame_draw(akariin.dither, akariin.frame_1);
  frame_end();
  
  camera_set_viewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  frame_draw(akariin.hdr, akariin.frame_2);
}
