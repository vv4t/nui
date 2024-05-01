#include <renderer/frame.h>

#include <stdlib.h>
#include <renderer/mesh.h>

#define MAX_CHANNEL 8

typedef struct {
  GLuint type;
  texture_t texture;
} channel_t;

struct framebuffer_s {
  int width;
  int height;
  texture_t texture;
  target_t target;
  channel_t channel[MAX_CHANNEL];
};

static mesh_t frame_mesh;

static framebuffer_t _framebuffer_create(int width, int height);

void frame_init()
{
  meshdata_t md = meshdata_create();
  meshdata_add_quad(md, identity());
  frame_mesh = vbuffer_add(md);
  meshdata_destroy(md);
}

void frame_draw(shader_t shader)
{
  shader_bind(shader);
  vbuffer_draw(frame_mesh);
}

void frame_shader_source(shaderdata_t sd, const char *path)
{
  shaderdata_source(sd, "assets/shader/vertex/frame.vert", SD_VERT);
  shaderdata_source(sd, path, SD_FRAG);
  shaderdata_text(sd, "in vec2 vs_uv;", SD_FRAG);
  shaderdata_text(sd, "layout (location = 0) out vec4 frag_color;", SD_FRAG);
  shaderdata_text(sd, "void main() { frame_shade(frag_color, vs_uv); }", SD_FRAG);
}

shader_t frame_shader_load(const char *path)
{
  shaderdata_t sd = shaderdata_create();
  frame_shader_source(sd, path); 
  shader_t shader = shader_load(sd);
  shaderdata_destroy(sd);
  return shader;
}

framebuffer_t framebuffer_create(int width, int height, texture_t depth_buffer)
{
  framebuffer_t fb= malloc(sizeof(*fb));
  fb->width = width;
  fb->height = height;
  fb->texture = texture_create(width, height, GL_RGBA, GL_UNSIGNED_BYTE);
  fb->target = target_create(
    2,
    GL_COLOR_ATTACHMENT0, fb->texture,
    GL_DEPTH_ATTACHMENT, depth_buffer
  );
  
  for (int i = 0; i < MAX_CHANNEL; i++) {
    fb->channel[i].texture = GL_INVALID_VALUE;
  }
  
  return fb;
}

void framebuffer_begin(framebuffer_t fb)
{
  target_bind(fb->target);
  glViewport(0, 0, fb->width, fb->height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void framebuffer_end()
{
  target_unbind();
}

void framebuffer_bind(framebuffer_t fb, int channel, GLuint type, texture_t texture)
{
  if (channel < 0 || channel >= MAX_CHANNEL) {
    return;
  }
  
  fb->channel[channel] = (channel_t) {
    .type = type,
    .texture = texture
  };
}

void framebuffer_update(framebuffer_t fb, shader_t shader)
{
  target_bind(fb->target);
  framebuffer_draw(fb, shader);
  target_unbind();
}

void framebuffer_draw(framebuffer_t fb, shader_t shader)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  for (int i = 0; i < MAX_CHANNEL; i++) {
    if (fb->channel[i].texture != GL_INVALID_VALUE) {
      texture_bind(fb->channel[i].texture, fb->channel[i].type, i);
    }
  }
  
  frame_draw(shader);
}

void framebuffer_destroy(framebuffer_t fb)
{
  texture_destroy(fb->texture);
  target_destroy(fb->target);
}

texture_t framebuffer_get_texture(framebuffer_t fb)
{
  return fb->texture;
}

target_t framebuffer_get_target(framebuffer_t fb)
{
  return fb->target;
}
