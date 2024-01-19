#include "test_pipeline.h"

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
  
  shader_setup_t shader_setup;
  shader_setup_init(&shader_setup, "test_pipeline");
  shader_setup_import(&shader_setup, SHADER_BOTH, "camera");
  shader_setup_import(&shader_setup, SHADER_BOTH, "material");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "light");
  shader_setup_source(&shader_setup, "light");
  
  if (!shader_setup_compile(&test_pipeline.shader, &shader_setup)) {
    return false;
  }
  
  camera_shader_setup(test_pipeline.shader);
  material_shader_setup(test_pipeline.shader);
  light_shader_setup(test_pipeline.shader);
  light_shader_setup_shadow(test_pipeline.shader);
  
  shader_setup_free(&shader_setup);
  
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
  
  return true;
}

void test_pipeline_pass()
{
  wave_update(&test_pipeline.wave);
  
  frame_begin(0);
  light_bind_depth_map(test_pipeline.shader);
  renderer_scene_pass();
  model_draw(&test_pipeline.model);
  frame_end();
  
  frame_begin(1);
  frame_draw(test_pipeline.dither, 0);
  frame_end();
  
  camera_set_viewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  frame_draw(test_pipeline.hdr, 1);
}
