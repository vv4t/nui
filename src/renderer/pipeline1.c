#include "pipeline1.h"

#include "api.h"
#include "camera.h"
#include "material.h"
#include "defer.h"
#include "frame.h"
#include "light.h"
#include "../gl/quad.h"
#include "../gl/gl.h"
#include "../gl/shader.h"

struct {
  GLuint shader;
  GLuint hdr;
  GLuint dither;
} pipeline1;

bool pipeline1_init()
{
  shader_setup_t shader_setup;
  shader_setup_init(&shader_setup, "pipeline1");
  shader_setup_import(&shader_setup, SHADER_BOTH, "camera");
  shader_setup_import(&shader_setup, SHADER_BOTH, "material");
  shader_setup_import(&shader_setup, SHADER_FRAGMENT, "light");
  shader_setup_source(&shader_setup, "light");
  
  if (!shader_setup_compile(&pipeline1.shader, &shader_setup)) {
    return false;
  }
  
  camera_shader_setup(pipeline1.shader);
  material_shader_setup(pipeline1.shader);
  light_shader_setup(pipeline1.shader);
  light_shader_setup_shadow(pipeline1.shader);
  
  shader_setup_free(&shader_setup);
  
  if (!frame_shader_load(&pipeline1.hdr, "hdr")) {
    return false;
  }
  
  if (!frame_shader_load(&pipeline1.dither, "dither")) {
    return false;
  }
  
  return true;
}

void pipeline1_pass()
{
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  
  frame_begin(0);
  glUseProgram(pipeline1.shader);
  light_bind_depth_map(pipeline1.shader);
  renderer_scene_pass();
  frame_end();
  
  camera_set_viewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  frame_draw(pipeline1.dither, 0);
}
