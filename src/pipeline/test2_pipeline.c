#include "test2_pipeline.h"

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
#include "../renderer/cubemap.h"
#include "../gl/quad.h"
#include "../gl/gl.h"
#include "../gl/shader.h"

struct {
  GLuint shader;
} test2_pipeline;

static bool test2_pipeline_defer_init();
static bool test2_pipeline_forward_init();

bool test2_pipeline_init()
{
  if (!skybox_init("night")) {
    return false;
  }
  
  if (!cubemap_init()) {
    return false;
  }
  
  shader_setup_t shader_setup;
  shader_setup_init(&shader_setup, "shader");
  shader_setup_import(&shader_setup, SHADER_BOTH, "camera");
  shader_setup_import(&shader_setup, SHADER_BOTH, "material");
  shader_setup_source(&shader_setup, "flat");
  
  if (!shader_setup_compile(&test2_pipeline.shader, &shader_setup)) {
    return false;
  }
  
  camera_shader_setup(test2_pipeline.shader);
  material_shader_setup(test2_pipeline.shader);
  
  shader_setup_free(&shader_setup);
  
  return true;
}

void test2_pipeline_pass()
{
  camera_set_viewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  
  /*
  skybox_render();
  
  vec3_t at = vec3_init(1.0, 0.0, 0.0);
  vec3_t from = vec3_init(-2.0, 1.0, -5.0);
  
  // camera_look_at(vec3_add(from, at), from, vec3_init(0.0, 1.0, 0.0));
  
  glUseProgram(test2_pipeline.shader);
  renderer_scene_pass();
  */
  
  skybox_render_texture(cubemap_get_texture());
}
