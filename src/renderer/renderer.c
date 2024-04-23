#include <renderer/renderer.h>

#include <renderer/meshdata.h>
#include <renderer/shaderdata.h>
#include <renderer/texture.h>
#include <renderer/shader.h>
#include <renderer/vbuffer.h>
#include <renderer/target.h>
#include <renderer/camera.h>
#include <GL/glew.h>

#define MAX_VERTICES 1024

struct {
  mesh_t mesh;
  shader_t shader;
  texture_t texture;
  texture_t buffer;
  target_t target;
} renderer;

void renderer_init()
{
  camera_init();
  vbuffer_init(MAX_VERTICES);
  vbuffer_bind();
  
  float aspect_ratio = 800.0 / 600.0;
  camera_perspective(1.0, aspect_ratio, 1.0, 100.0);
  
  glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
  
  meshdata_t md = meshdata_create();
  meshdata_add_quad(md, identity());
  renderer.mesh = vbuffer_add(md);
  
  renderer.texture = texture_image("assets/image/test.png");
  
  shaderdata_t sd = shaderdata_create();
  camera_shader_import(sd);
  shaderdata_source(sd, "assets/shader/shader.vert", S_VERT);
  shaderdata_source(sd, "assets/shader/shader.frag", S_FRAG);
  renderer.shader = shader_load(sd);
  camera_shader_attach(renderer.shader);
  
  renderer.buffer = texture_create_rgba_byte(800, 600);
  renderer.target = target_create(renderer.buffer);
  
  meshdata_destroy(md);
  shaderdata_destroy(sd);
}

float t = 0.0;

void renderer_render()
{
  t += 0.015;
  
  shader_bind(renderer.shader);
  
  matrix m = transform(vec3(0.0, 0.0, 3.0), vec3(0.0, t, 0.0), vec3(1.0, 1.0, 1.0));
  
  camera_move(vec3(cos(t), 0.0, 0.0), vec3(0.0, 0.0, 0.0));
  camera_update(m);
  
  target_bind(renderer.target);
  glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  texture_bind(renderer.texture, GL_TEXTURE_2D, 0);
  vbuffer_draw(renderer.mesh);
  target_unbind();
  
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  texture_bind(renderer.buffer, GL_TEXTURE_2D, 0);
  vbuffer_draw(renderer.mesh);
}

void renderer_deinit()
{
  texture_destroy(renderer.buffer);
  texture_destroy(renderer.texture);
  // target_destroy(renderer.target);
  shader_destroy(renderer.shader);
  vbuffer_deinit();
  camera_deinit();
}
