#include "forward_pipeline.h"

#include "../renderer/camera.h"
#include "../renderer/material.h"
#include "../renderer/defer.h"
#include "../renderer/frame.h"
#include "../renderer/light.h"
#include "../renderer/api.h"
#include "../gl/quad.h"
#include "../gl/gl.h"
#include "../gl/shader.h"

struct {
  GLuint shader;
  GLuint hdr;
  GLuint dither;
} forward_pipeline;

bool forward_pipeline_init()
{
  shader_setup_t shader_setup;
  shader_setup_init(&shader_setup, "forward_pipeline");
  shader_setup_import(&shader_setup, SHADER_BOTH, "camera");
  shader_setup_import(&shader_setup, SHADER_BOTH, "material");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "light");
  shader_setup_source(&shader_setup, "light");
  
  if (!shader_setup_compile(&forward_pipeline.shader, &shader_setup)) {
    return false;
  }
  
  camera_shader_setup(forward_pipeline.shader);
  material_shader_setup(forward_pipeline.shader);
  light_shader_setup(forward_pipeline.shader);
  light_shader_setup_shadow(forward_pipeline.shader);
  
  shader_setup_free(&shader_setup);
  
  if (!frame_shader_load(&forward_pipeline.hdr, "hdr")) {
    return false;
  }
  
  if (!frame_shader_load(&forward_pipeline.dither, "dither")) {
    return false;
  }
  
  return true;
}

void forward_pipeline_pass()
{
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  
  frame_begin(0);
  light_bind_depth_map(forward_pipeline.shader);
  renderer_scene_pass();
  frame_end();
  
  frame_begin(1);
  frame_draw(forward_pipeline.dither, 0);
  frame_end();
  
  camera_set_viewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  frame_draw(forward_pipeline.hdr, 1);
}
