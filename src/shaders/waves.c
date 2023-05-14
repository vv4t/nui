#include "waves.h"

#include "../common/log.h"
#include "../common/file.h"

bool waves_init(waves_t *waves, mesh_t quad_mesh)
{
  for (int i = 0; i < 2; i++) {
    glGenFramebuffers(1, &waves->fbo[i]);
    glBindFramebuffer(GL_FRAMEBUFFER, waves->fbo[i]);
    
    glGenTextures(1, &waves->wave[i]);
    
    glBindTexture(GL_TEXTURE_2D, waves->wave[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 512, 512, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    float border_color[] = { 0.5f, 0.5f, 0.0f, 0.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, waves->wave[i], 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  
  char *src_vertex = file_read_all("res/shader/hdr.vert");
  char *src_fragment = file_read_all("res/shader/waves.frag");

  if (!shader_load(&waves->shader, "", src_vertex, src_fragment)) {
    LOG_ERROR("failed to load shader");
    return false;
  }
  
  free(src_vertex);
  free(src_fragment);
  
  waves->quad_mesh = quad_mesh;
  
  return true;
}

void waves_move(waves_t *waves, full_bright_t *full_bright, view_t *view)
{
  glViewport(0, 0, 512, 512);
  
  glBindFramebuffer(GL_FRAMEBUFFER, waves->fbo[0]);
  
    glUseProgram(waves->shader);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, waves->wave[1]);
    
    draw_mesh(waves->quad_mesh);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  glBindFramebuffer(GL_FRAMEBUFFER, waves->fbo[1]);
    waves_show(waves, full_bright, view);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void waves_setup(waves_t *waves, colors_t *colors, view_t *view)
{
  glBindFramebuffer(GL_FRAMEBUFFER, waves->fbo[1]);
    glViewport(0, 0, 512, 512);
    glClearColor(0.5f, 0.5f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    colors_bind(colors);
    view_set(view, mat4x4_init_identity(), vec3_init(0.0, 0.0, 0.0));
    
    view_sub_data(
      view,
      mat4x4_init_transform(
        vec3_init(0.25f, 0.25f, 1.0f),
        vec3_init(0.1f, 0.1f, 0.0f)
      )
    );
    colors_set_color(colors, vec4_init(0.9f, 0.7f, 0.0f, 1.0f));
    draw_mesh(waves->quad_mesh);
    
    view_sub_data(
      view,
      mat4x4_init_transform(
        vec3_init(-0.25f, -0.25f, 1.0f),
        vec3_init(0.1f, 0.1f, 0.0f)
      )
    );
    colors_set_color(colors, vec4_init(0.9f, 0.5f, 0.0f, 1.0f));
    draw_mesh(waves->quad_mesh);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void waves_show(waves_t *waves, full_bright_t *full_bright, view_t *view)
{
  material_t material = { .diffuse = waves->wave[0] };
  
  glDisable(GL_DEPTH_TEST);
  
  full_bright_bind(full_bright);
  
  view_set(view, mat4x4_init_identity(), vec3_init(0.0, 0.0, 0.0));
  view_sub_data(view, mat4x4_init_identity());
  
  full_bright_set_material(&material);
  draw_mesh(waves->quad_mesh);
  
  glEnable(GL_DEPTH_TEST);
}
