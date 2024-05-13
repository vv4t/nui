#include <renderer/deferred.h>
#include <renderer/target.h>
#include <renderer/channel.h>
#include <renderer/frame.h>
#include <renderer/surface.h>
#include <renderer/camera.h>
#include <stdio.h>
#include <stdlib.h>

struct gbuffer_s {
  target_t target;
  channel_t channel;
};

struct deferred_s {
  gbuffer_t gb;
  channel_t channel;
  shader_t shader;
};

static surface_t sf_gbuffer;

void gbuffer_init()
{
  sf_gbuffer = surface_create();
  shaderdata_t sd = shaderdata_create();
  surface_shader_load(sf_gbuffer, sd, "assets/shader/surface/gbuffer.frag");
  shaderdata_destroy(sd);
}

void gbuffer_deinit()
{
  surface_destroy(sf_gbuffer);
}

gbuffer_t gbuffer_create(texture_t albedo, texture_t position, texture_t normal, texture_t depth)
{
  gbuffer_t gb = malloc(sizeof(*gb));
  gb->target = target_create(
    4,
    GL_COLOR_ATTACHMENT0, albedo,
    GL_COLOR_ATTACHMENT1, position,
    GL_COLOR_ATTACHMENT2, normal,
    GL_DEPTH_ATTACHMENT, depth
  );
  gb->channel = channel_create();
  channel_in(gb->channel, "in_albedo", GL_TEXTURE_2D, albedo);
  channel_in(gb->channel, "in_position", GL_TEXTURE_2D, position);
  channel_in(gb->channel, "in_normal", GL_TEXTURE_2D, normal);
  return gb;
}

void gbuffer_bind(gbuffer_t gb)
{
  target_bind(gb->target);
  surface_bind(sf_gbuffer);
}

void gbuffer_unbind(gbuffer_t gb)
{
  target_unbind();
}

void gbuffer_destroy(gbuffer_t gb)
{
  target_destroy(gb->target);
  channel_destroy(gb->channel);
  free(gb);
}

deferred_t deferred_create(gbuffer_t gb)
{
  deferred_t df = malloc(sizeof(*df));
  df->gb = gb;
  df->channel = channel_copy(df->gb->channel);
  return df;
}

void deferred_shader_load(deferred_t df, shaderdata_t sd, const char *path)
{
  camera_shader_import(sd);
  shaderdata_source(sd, "assets/shader/deferred/deferred.vert", SD_VERT);
  shaderdata_source(sd, path, SD_FRAG);
  shaderdata_text(sd, "in vec2 vs_uv;", SD_FRAG);
  shaderdata_text(sd, "uniform sampler2D in_albedo;", SD_FRAG);
  shaderdata_text(sd, "uniform sampler2D in_position;", SD_FRAG);
  shaderdata_text(sd, "uniform sampler2D in_normal;", SD_FRAG);
  shaderdata_text(sd, "layout (location = 0) out vec4 frag_color;", SD_FRAG);
  shaderdata_text(sd, "void main() { deferred_shade(frag_color, in_albedo, in_position, in_normal, vs_uv); }", SD_FRAG);
  
  df->shader = shader_load(sd);
  
  channel_shader_attach(df->shader, df->channel);
  camera_shader_attach(df->shader);
}

void deferred_display(deferred_t df)
{
  channel_bind(df->channel);
  channel_bind(df->gb->channel);
  frame_draw(df->shader);
}

void deferred_in(deferred_t df, const char *name, GLuint type, GLuint texture)
{
  channel_in(df->channel, name, type, texture);
}

void deferred_destroy(deferred_t df)
{
  shader_destroy(df->shader);
  channel_destroy(df->channel);
  free(df);
}

shader_t deferred_get_shader(deferred_t df)
{
  return df->shader;
}
