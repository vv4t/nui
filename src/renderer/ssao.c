#include "ssao.h"

#include "../common/nui_math.h"

void ssao_shader_setup(GLuint shader)
{
  glUseProgram(shader);
  
  vec3_t samples[64];
  
  for (int i = 0; i < 64; i++) {
    float x = (rand() % 256) / 256.0f * 2.0 - 1.0;
    float y = (rand() % 256) / 256.0f * 2.0 - 1.0;
    float z = (rand() % 256) / 256.0f;
    float t = (rand() % 256) / 256.0f;
    
    samples[i] = vec3_mulf(vec3_normalize(vec3_init(x, y, z)), t);
  }
  
  GLuint ul_samples = glGetUniformLocation(shader, "u_samples");
  glUniform3fv(ul_samples, 64, (float*) samples);
}
