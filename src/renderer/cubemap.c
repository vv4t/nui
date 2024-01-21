#include "cubemap.h"

#include "api.h"
#include "skybox.h"
#include "camera.h"
#include "material.h"
#include "../gl/shader.h"

#define CUBEMAP_SIZE 128

typedef struct {
  GLuint shader;
  GLuint texture;
  GLuint fbo;
  GLuint rbo;
} cubemap_t;

static cubemap_t cubemap;

bool cubemap_init()
{
  glGenTextures(1, &cubemap.texture);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.texture);
  
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 
  
  for (unsigned int i = 0; i < 6; i++) {
    glTexImage2D(
      GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
      CUBEMAP_SIZE, CUBEMAP_SIZE,
      0, GL_RGB, GL_UNSIGNED_BYTE, NULL
    );
  }
  
  glGenFramebuffers(1, &cubemap.fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, cubemap.fbo);
  
  glGenTextures(1, &cubemap.rbo);
  glBindTexture(GL_TEXTURE_2D, cubemap.rbo);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, CUBEMAP_SIZE, CUBEMAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, cubemap.rbo, 0);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  shader_setup_t shader_setup;
  shader_setup_init(&shader_setup, "shader");
  shader_setup_import(&shader_setup, SHADER_BOTH, "camera");
  shader_setup_import(&shader_setup, SHADER_BOTH, "material");
  shader_setup_source(&shader_setup, "flat");
  
  if (!shader_setup_compile(&cubemap.shader, &shader_setup)) {
    return false;
  }
  
  camera_shader_setup(cubemap.shader);
  material_shader_setup(cubemap.shader);
  
  shader_setup_free(&shader_setup);
}

void cubemap_pass(vec3_t view_pos)
{
  vec3_t at[] = {
    vec3_init(-1.0,  0.0,  0.0),
    vec3_init( 1.0,  0.0,  0.0),
    vec3_init( 0.0,  1.0,  0.0),
    vec3_init( 0.0, -1.0,  0.0),
    vec3_init( 0.0,  0.0,  1.0),
    vec3_init( 0.0,  0.0, -1.0)
  };
  
  vec3_t up[] = {
    vec3_init(0.0, -1.0,  0.0),
    vec3_init(0.0, -1.0,  0.0),
    vec3_init(0.0,  0.0,  1.0),
    vec3_init(0.0,  0.0, -1.0),
    vec3_init(0.0, -1.0,  0.0),
    vec3_init(0.0, -1.0,  0.0)
  };
  
  GLuint faces[] = {
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
  };
  
  glBindFramebuffer(GL_FRAMEBUFFER, cubemap.fbo);
  
  view_t view;
  view_set_perspective(&view, 1.0, to_radians(90.0), 0.1, 100.0);
  
  for (int i = 0; i < 6; i++) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, faces[i], cubemap.texture, 0);
    
    camera_set_viewport(0, 0, CUBEMAP_SIZE, CUBEMAP_SIZE);
    camera_set_view(view);
    camera_look_at(vec3_add(view_pos, at[i]), view_pos, up[i]);
    
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    
    skybox_render();
    glUseProgram(cubemap.shader);
    renderer_shadow_pass();
    
  }
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint cubemap_get_texture()
{
  return cubemap.texture;
}
