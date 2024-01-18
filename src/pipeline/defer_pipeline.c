#include "defer_pipeline.h"

#include "../renderer/camera.h"
#include "../renderer/material.h"
#include "../renderer/defer.h"
#include "../renderer/frame.h"
#include "../renderer/light.h"
#include "../renderer/ssao.h"
#include "../renderer/api.h"
#include "../gl/quad.h"
#include "../gl/gl.h"
#include "../gl/shader.h"

struct {
  GLuint shader;
  GLuint hdr;
  GLuint dither;
} defer_pipeline;

bool defer_pipeline_init()
{
  shader_setup_t shader_setup;
  shader_setup_init(&shader_setup, "defer_pipeline");
  shader_setup_import(&shader_setup, SHADER_BOTH, "camera");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "light");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "ssao");
  defer_shader_source(&shader_setup, "light");
  
  if (!shader_setup_compile(&defer_pipeline.shader, &shader_setup)) {
    return false;
  }
  
  ssao_shader_setup(defer_pipeline.shader);
  defer_shader_setup(defer_pipeline.shader);
  camera_shader_setup(defer_pipeline.shader);
  light_shader_setup(defer_pipeline.shader);
  light_shader_setup_shadow(defer_pipeline.shader);
  
  shader_setup_free(&shader_setup);
  
  if (!frame_shader_load(&defer_pipeline.hdr, "hdr")) {
    return false;
  }
  
  if (!frame_shader_load(&defer_pipeline.dither, "dither")) {
    return false;
  }
  
  return true;
}

void defer_pipeline_pass()
{
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  
  defer_begin();
  renderer_scene_pass();
  defer_end();
  
  frame_begin(0);
  light_bind_depth_map(defer_pipeline.shader);
  defer_draw(defer_pipeline.shader);
  frame_end();
  
  frame_begin(1);
  frame_draw(defer_pipeline.dither, 0);
  frame_end();
  
  camera_set_viewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  frame_draw(defer_pipeline.hdr, 1);
}
