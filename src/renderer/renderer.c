#include <renderer/renderer.h>

#include <renderer/frame.h>
#include <renderer/texture.h>
#include <renderer/shader.h>
#include <renderer/mesh.h>
#include <renderer/target.h>
#include <renderer/camera.h>
#include <GL/glew.h>

#define MAX_VERTICES 1024

struct {
  mesh_t mesh;
  texture_t texture;
  
  framebuffer_t fb[2];
  
  shader_t hdr;
  shader_t dither;
} renderer;

void renderer_init()
{
  camera_init();
  vbuffer_init(MAX_VERTICES);
  vbuffer_bind();
  frame_init();
  
  float aspect_ratio = 800.0 / 600.0;
  camera_perspective(1.0, aspect_ratio, 1.0, 100.0);
  
  glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
  
  renderer.texture = texture_image("assets/image/test.png");
  
  renderer.fb[0] = framebuffer_create(400, 300);
  renderer.fb[1] = framebuffer_create(400, 300);
  
  framebuffer_bind(renderer.fb[0], 0, GL_TEXTURE_2D, framebuffer_get_texture(renderer.fb[1]));
  framebuffer_bind(renderer.fb[1], 0, GL_TEXTURE_2D, framebuffer_get_texture(renderer.fb[0]));
  
  renderer.hdr = frame_shader_load("assets/shader/frame/hdr.frag");
  renderer.dither = frame_shader_load("assets/shader/frame/dither.frag");
}

void renderer_render()
{
  framebuffer_begin(renderer.fb[0]);
  texture_bind(renderer.texture, GL_TEXTURE_2D, 0);
  frame_draw(renderer.hdr);
  framebuffer_end();
  
  glViewport(0, 0, 800, 600);
  
  framebuffer_draw(renderer.fb[1], renderer.dither);
}

void renderer_deinit()
{
  framebuffer_destroy(renderer.fb[0]);
  framebuffer_destroy(renderer.fb[1]);
  texture_destroy(renderer.texture);
  vbuffer_deinit();
  camera_deinit();
}
