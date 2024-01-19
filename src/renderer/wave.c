#include "wave.h"

#include "camera.h"
#include "../gl/quad.h"
#include "../gl/shader.h"

typedef struct {
  GLuint shader;
} wave_def_t;

static wave_def_t wave_def;

bool wave_init()
{
  shader_setup_t shader_setup;
  shader_setup_init(&shader_setup, "wave");
  shader_setup_source(&shader_setup, "wave");
  
  if (!shader_setup_compile(&wave_def.shader, &shader_setup)) {
    return false;
  }
  
  shader_setup_free(&shader_setup);
  
  return true;
}


void wave_new(wave_t *wave, int width, int height)
{
  wave->width = width;
  wave->height = height;
  wave->turn = 0;
  
  glGenTextures(1, &wave->normal);
  glBindTexture(GL_TEXTURE_2D, wave->normal);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glGenFramebuffers(2, wave->fbo);
  glGenTextures(2, wave->buffer);
  
  float *data = calloc(width * height * 4, sizeof(float));
  
  for (int i = 0; i < 10; i++) {
    int size = 20;
    int xd = rand() % (wave->width - size);
    int yd = rand() % (wave->width - size);
  
    for (int y = 0; y < size; y++) {
      for (int x = 0; x < size; x++) {
        float xt = (x - size / 2.0) / size;
        float yt = (y - size / 2.0) / size;
        
        float d = sqrt(xt * xt + yt * yt);
        float t = cos(d);
        
        int xp = x + xd;
        int yp = y + yd;
        
        data[(xp + yp * width) * 4 + 0] = t;
        data[(xp + yp * width) * 4 + 1] = 0.0f;
        data[(xp + yp * width) * 4 + 2] = 0.0f;
        data[(xp + yp * width) * 4 + 3] = 1.0f;
      }
    }
  }
  
  for (int i = 0; i < 2; i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, wave->fbo[i]);
    
    glBindTexture(GL_TEXTURE_2D, wave->buffer[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, wave->buffer[i], 0);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, wave->normal, 0);
    
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
  }
  
  free(data);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void wave_update(wave_t *wave)
{
  int next_turn = (wave->turn + 1) % 2;
  
  glBindFramebuffer(GL_FRAMEBUFFER, wave->fbo[next_turn]);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glUseProgram(wave_def.shader);
  
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, wave->buffer[wave->turn]);
  
  camera_set_viewport(0, 0, wave->width, wave->height);
  quad_draw();
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  wave->turn = next_turn;
}
