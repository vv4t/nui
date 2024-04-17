#include <renderer/renderer.h>

#include <renderer/meshdata.h>
#include <renderer/shaderdata.h>
#include <renderer/texture.h>
#include <renderer/shader.h>
#include <renderer/vbuffer.h>
#include <renderer/camera.h>
#include <GL/glew.h>

#define MAX_VERTICES 1024

struct {
  mesh_t mesh;
  shader_t shader;
  texture_t texture;
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
  
  texture_t buffer1 = texture_create_rgb32u(WIDTH, HEIGHT);
  texture_t buffer2 = texture_create_rgb32u(WIDTH, HEIGHT);
  
  target_t target_ping = target_create(GL_COLOR_ATTACHMENT0, buffer1);
  target_t target_pong = target_create(GL_COLOR_ATTACHMENT0, buffer2);
  
  // RENDER
  
  target_bind(target_ping);
  shader_bind(renderer.hdr);
  vbuffer_draw(renderer.quad);
  target_unbind();
  
  target_bing(target_pong);
  shader_bind(renderer.hdr);
  vbuffer_draw(renderer.quad);
  target_unbind();
  
  meshdata_destroy(md);
  shaderdata_destroy(sd);
}

float t = 0.0;

void renderer_render()
{
  t += 0.015;
  
  shader_bind(renderer.shader);
  texture_bind(renderer.texture, GL_TEXTURE_2D, 0);
  
  matrix m = transform(vec3(0.0, 0.0, 3.0), vec3(0.0, t, 0.0), vec3(1.0, 1.0, 1.0));
  
  camera_move(vec3(cos(t), 0.0, 0.0), vec3(0.0, 0.0, 0.0));
  camera_update(m);
  
  glClear(GL_COLOR_BUFFER_BIT);
  vbuffer_draw(renderer.mesh);
}

void renderer_deinit()
{
  texture_destroy(renderer.texture);
  shader_destroy(renderer.shader);
  vbuffer_deinit();
  camera_deinit();
}

/*
  texture_t albedo = texture_create_rgba32u(WIDTH, HEIGHT);
  texture_t pos = texture_create_rgba16f(WIDTH, HEIGHT);
  texture_t normal = texture_create_rgba16f(WIDTH, HEIGHT);
  
  shaderinput_t s_in = shaderinput_create();
  shaderinput_add(s_in, 0, "u_albedo", GL_TEXTURE_2D);
  shaderinput_add(s_in, 1, "u_normal", GL_TEXTURE_2D);
  shaderinput_add(s_in, 2, "u_skybox", GL_TEXTURE_CUBE_MAP);
  
  target_t deferred = target_create();
  target_output(deferred, GL_COLOR_ATTACHMENT0, albedo);
  target_output(deferred, GL_COLOR_ATTACHMENT1, pos);
  target_output(deferred, GL_COLOR_ATTACHMENT2, normal);
  
  shader_t shader = shader_create();
  
  render() {
    frame_bind(deferred);
    shader_bind(shader);
    vbuffer_draw(quad);
  }
  
  ---------------------------------------------------------------
  
  texture_t depth = texture_create_depth(800, 600);
  
  frame_t light = frame_create();
  frame_output(light, GL_DEPH_ATTACHMENT, depth);

*/
