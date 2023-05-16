#include "waves.h"

#include "../common/log.h"
#include "../common/file.h"

#define WAVES_SIZE 256

bool waves_init(waves_t *waves, mesh_t quad_mesh)
{
  for (int i = 0; i < 3; i++) {
    glGenFramebuffers(1, &waves->fbo[i]);
    glBindFramebuffer(GL_FRAMEBUFFER, waves->fbo[i]);
    
    glGenTextures(1, &waves->wave[i]);
    
    glBindTexture(GL_TEXTURE_2D, waves->wave[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WAVES_SIZE, WAVES_SIZE, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, waves->wave[i], 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  
  char *src_vertex = file_read_all("res/shader/hdr.vert");
  char *src_fragment = file_read_all("res/shader/waves.frag");

  if (!shader_load(&waves->shader, "", src_vertex, src_fragment)) {
    LOG_ERROR("failed to load shader");
    return false;
  }
  
  char *src_out = file_read_all("res/shader/waves_out.frag");
  
  if (!shader_load(&waves->out_shader, "", src_vertex, src_out)) {
    LOG_ERROR("failed to load shader");
    return false;
  }
  
  free(src_vertex);
  free(src_fragment);
  free(src_out);
  
  waves->quad_mesh = quad_mesh;
  
  return true;
}

void waves_move(waves_t *waves, full_bright_t *full_bright, view_t *view)
{
  glDisable(GL_DEPTH_TEST);
  
  glViewport(0, 0, WAVES_SIZE, WAVES_SIZE);
  
  glScissor(1, 1, WAVES_SIZE - 2, WAVES_SIZE - 2);
  glEnable(GL_SCISSOR_TEST);
  glBindFramebuffer(GL_FRAMEBUFFER, waves->fbo[0]);
    glUseProgram(waves->shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, waves->wave[1]);
    draw_mesh(waves->quad_mesh);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDisable(GL_SCISSOR_TEST);
  
  glBindFramebuffer(GL_FRAMEBUFFER, waves->fbo[1]);
    material_t material = { .diffuse = waves->wave[0] };
    full_bright_bind(full_bright);
    view_set(view, mat4x4_init_identity(), vec3_init(0.0, 0.0, 0.0));
    view_sub_data(view, mat4x4_init_identity());
    full_bright_set_material(&material);
    draw_mesh(waves->quad_mesh);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  glBindFramebuffer(GL_FRAMEBUFFER, waves->fbo[2]);
    glUseProgram(waves->out_shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, waves->wave[1]);
    draw_mesh(waves->quad_mesh);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  glEnable(GL_DEPTH_TEST);
}

void waves_setup(waves_t *waves, full_bright_t *full_bright, view_t *view)
{
  float data[64 * 64 * 4];
  for (int i = 0; i < 64; i++) {
    for (int j = 0; j < 64; j++) {
      float x = i - 32.0;
      float y = j - 32.0;
      
      float t = sqrt(x*x + y*y);
        
      if (t > 32) {
        data[(i * 64 + j) * 4 + 0] = 0.5;
        data[(i * 64 + j) * 4 + 1] = 0.5;
        data[(i * 64 + j) * 4 + 2] = 0;
        data[(i * 64 + j) * 4 + 3] = 1.0;
      } else {
        float theta = t / 32.0 * M_PI;
        
        data[(i * 64 + j) * 4 + 0] = sin(theta) * 0.5 * 0.5 + 0.5;
        data[(i * 64 + j) * 4 + 1] = cos(theta) * 0.5 * 0.5 + 0.5;
        data[(i * 64 + j) * 4 + 2] = 0;
        data[(i * 64 + j) * 4 + 3] = 1.0;
      }
    }
  }
  
  GLuint wave_pattern;
  glGenTextures(1, &wave_pattern);
  glBindTexture(GL_TEXTURE_2D, wave_pattern);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 64, 64, 0, GL_RGBA, GL_FLOAT, data);
  
  material_t material = { .diffuse = wave_pattern };
  
  glBindFramebuffer(GL_FRAMEBUFFER, waves->fbo[1]);
    glViewport(0, 0, WAVES_SIZE, WAVES_SIZE);
    glClearColor(0.5f, 0.5f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    full_bright_bind(full_bright);
    view_set(view, mat4x4_init_identity(), vec3_init(0.0, 0.0, 0.0));
    
    for (int i = 0; i < 1; i++) {
      float x = 0.0;//(rand() % 256) / 128.0 - 1.0;
      float y = 0.0;//(rand() % 256) / 128.0 - 1.0;
      float u = 0.2;// 0.05 + (rand() % 256) / 256.0 * 0.1;
      float t = 0.2;// 0.05 + (rand() % 256) / 256.0 * 0.1;
      
      view_sub_data(
        view,
        mat4x4_init_transform(
          vec3_init(x, y, 1.0f),
          vec3_init(u, t, 1.0f)
        )
      );
      full_bright_set_material(&material);
      draw_mesh(waves->quad_mesh);
    }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  glDeleteTextures(1, &wave_pattern);
}

void waves_show(waves_t *waves, full_bright_t *full_bright, view_t *view)
{
  glViewport(0, 0, 800, 800);
  
  material_t material = { .diffuse = waves->wave[2] };
  
  glDisable(GL_DEPTH_TEST);
  
  full_bright_bind(full_bright);
  
  view_set(view, mat4x4_init_identity(), vec3_init(0.0, 0.0, 0.0));
  view_sub_data(view, mat4x4_init_identity());
  
  full_bright_set_material(&material);
  draw_mesh(waves->quad_mesh);
  
  glEnable(GL_DEPTH_TEST);
}
