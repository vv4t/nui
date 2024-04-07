#include <renderer/renderer.h>

#include <renderer/meshdata.h>
#include <renderer/shaderdata.h>
#include <renderer/shader.h>
#include <renderer/vbuffer.h>
#include <GL/glew.h>

#define MAX_VERTICES 1024

struct ub_camera {
  matrix mvp;
};

struct {
  GLuint ubo;
} camera;

struct {
  mesh_t mesh;
  shader_t shader;
} renderer;

void camera_init()
{
  glGenBuffers(1, &camera.ubo);
  glBindBuffer(GL_UNIFORM_BUFFER, camera.ubo);
  glBufferData(GL_UNIFORM_BUFFER, 512, NULL, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, camera.ubo); 
}

void camera_update(matrix mvp)
{
  struct ub_camera ub_camera = {
    .mvp = mvp
  };
  
  glBindBuffer(GL_UNIFORM_BUFFER, camera.ubo);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ub_camera), &ub_camera);
}

void camera_deinit()
{
  glDeleteBuffers(1, &camera.ubo);
}

void renderer_init()
{
  camera_init();
  vbuffer_init(MAX_VERTICES);
  vbuffer_bind();
  
  glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
  
  meshdata_t md = meshdata_create();
  meshdata_add_quad(md, identity());
  renderer.mesh = vbuffer_add(md);
  
  shaderdata_t sd = shaderdata_create();
  shaderdata_line(sd, "#version 300 es", S_VERT | S_FRAG);
  shaderdata_line(sd, "precision mediump float;", S_FRAG);
  shaderdata_source(sd, "assets/shader/shader.vert", S_VERT);
  shaderdata_source(sd, "assets/shader/shader.frag", S_FRAG);
  renderer.shader = shader_load(sd);
  
  shader_bind(renderer.shader);
  
  GLuint ubl_camera = glGetUniformBlockIndex(renderer.shader, "ub_camera");
  glUniformBlockBinding(renderer.shader, ubl_camera, 0);
  
  meshdata_destroy(md);
  shaderdata_destroy(sd);
}

float t = 0.0;

void renderer_render()
{
  t += 0.015;
  
  matrix p = perspective(1.0, 1.0, 1.0, 100.0);
  matrix v = identity();
  matrix m = transform(vec3(0.0, 0.0, 3.0), vec3(0.0, t * 4.0, 0.0), vec3(1.0, 1.0, 1.0));
  
  matrix mvp = mdotm(m, mdotm(v, p));
  
  camera_update(mvp);
  
  glClear(GL_COLOR_BUFFER_BIT);
  vbuffer_draw(renderer.mesh);
}

void renderer_deinit()
{
  shader_destroy(renderer.shader);
  vbuffer_deinit();
  camera_deinit();
}
