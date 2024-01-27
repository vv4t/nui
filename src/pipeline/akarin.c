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

static bool akarin_init();
static void akarin_setup();
static void akarin_pass();

pipeline_t pipeline_akarin = {
  .init = akarin_init,
  .setup = akarin_setup,
  .pass = akarin_pass
};

static struct {
  GLuint shader;
  GLuint hdr;
  GLuint dither;
  frame_t frame_1;
  frame_t frame_2;
} akarin;

static bool akarin_init()
{
  if (!skybox_init("night")) {
    return false;
  }
  
  frame_new(&akarin.frame_1, VIEW_WIDTH, VIEW_HEIGHT);
  frame_new(&akarin.frame_2, VIEW_WIDTH, VIEW_HEIGHT);
  
  shader_setup_t shader_setup;
  shader_setup_init(&shader_setup, "akarin");
  shader_setup_import(&shader_setup, SHADER_BOTH, "camera");
  shader_setup_import(&shader_setup, SHADER_BOTH, "material");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "light");
  
  if (!shader_setup_source(&shader_setup, "assets/pipeline/akarin", "forward")) {
    return false;
  }
  
  if (!shader_setup_compile(&akarin.shader, &shader_setup)) {
    return false;
  }
  
  camera_shader_setup(akarin.shader);
  material_shader_setup(akarin.shader);
  light_shader_setup(akarin.shader);
  light_shader_setup_shadow(akarin.shader);
  
  glUseProgram(akarin.shader);
  GLuint ul_skybox = glGetUniformLocation(akarin.shader, "u_skybox");
  glUniform1i(ul_skybox, TEXTURE_SKYBOX_BINDING);
  
  shader_setup_free(&shader_setup);
  
  if (!frame_shader_load(&akarin.hdr, "hdr")) {
    return false;
  }
  
  if (!frame_shader_load(&akarin.dither, "dither")) {
    return false;
  }
  
  return true;
}

static void akarin_setup()
{
  
}

static void akarin_pass()
{
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  
  frame_begin(akarin.frame_1);
  skybox_render();
  light_bind_depth_map(akarin.shader);
  renderer_scene_pass();
  frame_end();
  
  frame_begin(akarin.frame_2);
  frame_draw(akarin.dither, akarin.frame_1);
  frame_end();
  
  camera_set_viewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  frame_draw(akarin.hdr, akarin.frame_2);
  
  /*
  glUseProgram(akarin.hdr);
  light_bind_depth_map(akarin.hdr);
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  camera_set_viewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  quad_draw();
  */
}
