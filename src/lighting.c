#include "light.h"

void lighting_init(lighting_t *lighting)
{
  lighting_init_shader(lighting);
  lighting_init_shadow(lighting);
  lighting_init_lights(lighting);
}

void lighting_bind(lighting_t *lighting)
{
  glUseProgram(lighting->light_shader);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_CUBE_MAP, renderer->depth_cube_map);
}

void lighting_shadow_pass(lighting_t *lighting)
{
  mat4x4_t projection_matrix = mat4x4_init_perspective(1.0, to_radians(90), 0.1, 100.0);
  mat4x4_t rotation_matrices[] = {
    mat4x4_init_rotation(quat_init_rotation(vec3_init(0.0, 1.0, 0.0), to_radians(+90))),  // left
    mat4x4_init_rotation(quat_init_rotation(vec3_init(0.0, 1.0, 0.0), to_radians(-90))),  // right
    
    mat4x4_init_rotation(
      quat_mul(
        quat_init_rotation(vec3_init(1.0, 0.0, 0.0), to_radians(-90)),
        quat_init_rotation(vec3_init(0.0, 1.0, 0.0), to_radians(180))
      )
    ), // down
    
    mat4x4_init_rotation(
      quat_mul(
        quat_init_rotation(vec3_init(1.0, 0.0, 0.0), to_radians(+90)),
        quat_init_rotation(vec3_init(0.0, 1.0, 0.0), to_radians(180))
      )
    ), // up
    
    mat4x4_init_rotation(quat_init_rotation(vec3_init(0.0, 1.0, 0.0), to_radians(180))),  // front
    mat4x4_init_rotation(quat_init_rotation(vec3_init(0.0, 1.0, 0.0), to_radians(0)))     // back
  };
  
  glViewport(0, 0, 1024, 1024);
  glBindFramebuffer(GL_FRAMEBUFFER, lighting->depth_map_fbo);
  
  glUseProgram(lighting->shadow_shader);
  
  mat4x4_t translation_matrix = mat4x4_init_translation(vec3_mulf(light_pos, -1));
  
  for (int i = 0; i < 6; i++) {
    mat4x4_t light_matrix = mat4x4_mul(translation_matrix, rotation_matrices[i]);
    mat4x4_t light_projection_matrix = mat4x4_mul(light_matrix, projection_matrix);
    mat4x4_t model_matrix = mat4x4_init_identity();
    mat4x4_t mvp_matrix = mat4x4_mul(model_matrix, light_projection_matrix);
    
    ubo_matrices_t ubo_matrices = {
      .model = model_matrix,
      .mvp = mvp_matrix,
      .view_pos = light_pos
    };
    
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ubo_matrices_t), &ubo_matrices);
    
    GLuint face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, face, renderer->depth_cube_map, 0);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, renderer->scene_mesh.ptr, renderer->scene_mesh.num_vertices);
  }
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  glViewport(0, 0, 1280, 720);
}

static void renderer_init_lights(renderer_t *renderer)
{
  glGenBuffers(1, &renderer->ubo_lights);
  glBindBuffer(GL_UNIFORM_BUFFER, renderer->ubo_lights);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(ubo_lights_t), NULL, GL_DYNAMIC_DRAW);
  
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, renderer->ubo_lights); 
  
  light_t empty[8] = {0};
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(empty), &empty);
}

void lighting_set_light(renderer_t *renderer, int light_id, vec3_t pos, float intensity, vec4_t color)
{
  light_t light = {
    .pos = pos,
    .intensity = intensity,
    .color = color
  };
  
  glBindBuffer(GL_UNIFORM_BUFFER, renderer->ubo_lights);
  glBufferSubData(GL_UNIFORM_BUFFER, light_id * sizeof(light_t), sizeof(light_t), &light);
  glBindBuffer(GL_UNIFORM_BUFFER, renderer->ubo_matrices);
}

void lighting_init_shadow()
{
  glGenFramebuffers(1, &renderer->depth_map_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, renderer->depth_map_fbo);
  
  glGenTextures(1, &renderer->depth_cube_map);
  glBindTexture(GL_TEXTURE_CUBE_MAP, renderer->depth_cube_map);
  
  for (int i = 0; i < 6; i++) { 
    glTexImage2D(
      GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
      0,
      GL_R32F,
      1024, 1024,
      0,
      GL_RED,
      GL_FLOAT, NULL
    );
  }
  
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  glBindFramebuffer(GL_FRAMEBUFFER, renderer->depth_map_fbo);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderer->depth_cube_map, 0);
  glDrawBuffer(GL_COLOR_ATTACHMENT0);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);  
  
  char *src_vertex = file_read_all("res/shader/shadow.vert");
  char *src_fragment = file_read_all("res/shader/shadow.frag");
  
  if (!shader_load(&renderer->shadow_shader, src_vertex, NULL, src_fragment))
    return false;
  
  free(src_vertex);
  free(src_fragment);
  
  GLuint ubl_matrices = glGetUniformBlockIndex(renderer->shadow_shader, "ubo_matrices");
  glUniformBlockBinding(renderer->shadow_shader, ubl_matrices, 0);
  
  return true;
}

void lighting_init_shader()
{
  char *src_vertex = file_read_all("res/shader/light.vert");
  char *src_fragment = file_read_all("res/shader/light.frag");
  
  if (!shader_load(&light->light_shader, src_vertex, NULL, src_fragment))
    return false;
  
  free(src_vertex);
  free(src_fragment);
  
  GLuint ubl_matrices = glGetUniformBlockIndex(main_shader->program, "ubo_matrices");
  GLuint ubl_lights = glGetUniformBlockIndex(main_shader->program, "ubo_lights");
  
  GLuint ul_color = glGetUniformLocation(main_shader->program, "u_color");
  GLuint ul_normal = glGetUniformLocation(main_shader->program, "u_normal");
  GLuint ul_depth_map = glGetUniformLocation(main_shader->program, "u_depth_cube_map");
  
  glUseProgram(main_shader->program);
  glUniform1i(ul_color, 0);
  glUniform1i(ul_normal, 1);
  glUniform1i(ul_depth_map, 2);
  
  glUniformBlockBinding(main_shader->program, ubl_matrices, 0);
  glUniformBlockBinding(main_shader->program, ubl_lights, 1);
  
  return true;
}
