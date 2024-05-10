#include <renderer/frame.h>

#include <stdlib.h>
#include <renderer/mesh.h>

#define MAX_CHANNEL 8

typedef struct {
  GLuint type;
  texture_t texture;
} channel_t;

struct frame_s {
  int width;
  int height;
  texture_t texture;
  target_t target;
  channel_t channel[MAX_CHANNEL];
};

static mesh_t frame_mesh;

static frame_t _frame_create(int width, int height);

void frame_init()
{
  meshdata_t md = meshdata_create();
  meshdata_add_quad(md, identity());
  frame_mesh = vbuffer_add(md);
  meshdata_destroy(md);
}

void frame_shader_source(shaderdata_t sd, const char *path)
{
  shaderdata_source(sd, "assets/shader/frame/frame.vert", SD_VERT);
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

frame_t frame_create(int width, int height, texture_t depth_buffer)
{
  frame_t fr = malloc(sizeof(*fr));
  fr->width = width;
  fr->height = height;
  fr->texture = texture_create(width, height, GL_RGBA, GL_UNSIGNED_BYTE);
  fr->target = target_create(
    2,
    GL_COLOR_ATTACHMENT0, fr->texture,
    GL_DEPTH_ATTACHMENT, depth_buffer
  );
  
  for (int i = 0; i < MAX_CHANNEL; i++) {
    fr->channel[i].texture = GL_INVALID_VALUE;
  }
  
  return fr;
}

void frame_begin(frame_t fr)
{
  target_bind(fr->target);
  glViewport(0, 0, fr->width, fr->height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void frame_end()
{
  target_unbind();
}

void frame_bind(frame_t fr, int channel, GLuint type, texture_t texture)
{
  if (channel < 0 || channel >= MAX_CHANNEL) {
    return;
  }
  
  fr->channel[channel] = (channel_t) {
    .type = type,
    .texture = texture
  };
}

void frame_update(frame_t fr, shader_t shader)
{
  target_bind(fr->target);
  frame_draw(fr, shader);
  target_unbind();
}

void frame_draw(frame_t fr, shader_t shader)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  for (int i = 0; i < MAX_CHANNEL; i++) {
    if (fr->channel[i].texture != GL_INVALID_VALUE) {
      texture_bind(fr->channel[i].texture, fr->channel[i].type, i);
    }
  }
  
  shader_bind(shader);
  vbuffer_draw(frame_mesh);
}

void frame_destroy(frame_t fr)
{
  texture_destroy(fr->texture);
  target_destroy(fr->target);
}

texture_t frame_get_texture(frame_t fr)
{
  return fr->texture;
}
