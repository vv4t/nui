#include <renderer/renderer.h>

#include <renderer/meshdata.h>
#include <renderer/shaderdata.h>
#include <renderer/shader.h>
#include <renderer/vbuffer.h>
#include <GL/glew.h>

#define MAX_VERTICES 1024

struct {
  mesh_t mesh;
  shader_t shader;
  GLuint u_mvp;
} renderer;

void renderer_init()
{
  vbuffer_init(MAX_VERTICES);
  vbuffer_bind();
  
  glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
  
  meshdata_t md = meshdata_create();
  meshdata_add_quad(md, identity());
  renderer.mesh = vbuffer_add(md);
  
  shaderdata_t sd = shaderdata_create();
  shaderdata_source(sd, "assets/shader/shader.vert", S_VERT);
  shaderdata_source(sd, "assets/shader/shader.frag", S_FRAG);
  renderer.shader = shader_load(sd);
  
  renderer.u_mvp = glGetUniformLocation(renderer.shader, "u_mvp");
  
  glUseProgram(renderer.shader);
  
  meshdata_destroy(md);
  shaderdata_destroy(sd);
}

float t = 0.0;

void renderer_render()
{
  t += 0.015;
  
  matrix m = transform(vec3(cos(t), 0.0, 0.0), vec3(0.0, 0.0, t), vec3(0.5, 0.1, 0.1));
  // matrix m = mdotm(scale(vec3(0.5, 0.1, 0.1)), mdotm(rotate_z(t), translate(vec3(cos(t), 0.0, 0.0))));
  glUniformMatrix4fv(renderer.u_mvp, 1, GL_FALSE, m.m);
  
  glClear(GL_COLOR_BUFFER_BIT);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void renderer_deinit()
{
  vbuffer_deinit();
}
