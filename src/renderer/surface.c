#include <renderer/surface.h>
#include <renderer/camera.h>
#include <renderer/channel.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct surface_s {
  GLuint shader;
  channel_t channel;
};

surface_t surface_create()
{
  surface_t sf = malloc(sizeof(*sf));
  sf->shader = GL_INVALID_VALUE;
  sf->channel = channel_create();
  channel_disable(sf->channel, 0);
  channel_disable(sf->channel, 1);
  return sf;
}

void surface_shader_load(surface_t sf, shaderdata_t sd, const char *path)
{
  camera_shader_import(sd);
  shaderdata_text(sd, "#define PLANAR_UV\n", SD_VERT);
  shaderdata_source(sd, "assets/shader/surface/surface.vert", SD_VERT);
  shaderdata_source(sd, path, SD_FRAG);
  shaderdata_text(sd, "in vec2 vs_uv;", SD_FRAG);
  shaderdata_text(sd, "in vec3 vs_frag_pos;", SD_FRAG);
  shaderdata_text(sd, "in vec3 vs_world_pos;", SD_FRAG);
  shaderdata_text(sd, "in mat3 vs_TBN;", SD_FRAG);
  shaderdata_text(sd, "uniform sampler2D u_albedo;", SD_FRAG);
  shaderdata_text(sd, "uniform sampler2D u_normal;", SD_FRAG);
  shaderdata_text(
    sd,
    "void main() {\
      vec3 normal = normalize(vs_TBN * (texture(u_normal, vs_uv).rgb * 2.0 - 1.0));\
      surface_shade(vs_frag_pos, vs_world_pos, vs_uv, normal, u_albedo);\
    }",
    SD_FRAG
  );
  
  sf->shader = shader_load(sd);
  camera_shader_attach(sf->shader);
  channel_shader_attach(sf->shader, sf->channel);
  
  shader_bind(sf->shader);
  glUniform1i(glGetUniformLocation(sf->shader, "u_albedo"), 0); 
  glUniform1i(glGetUniformLocation(sf->shader, "u_normal"), 1);
}

void surface_in(surface_t sf, const char *name, GLuint type, GLuint texture)
{
  channel_in(sf->channel, name, type, texture);
}

void surface_bind(surface_t sf)
{
  shader_bind(sf->shader);
  channel_bind(sf->channel);
}

void surface_destroy(surface_t sf)
{
  shader_destroy(sf->shader);
  channel_destroy(sf->channel);
  free(sf);
}

shader_t surface_get_shader(surface_t sf)
{
  return sf->shader;
}

material_t material_create(texture_t albedo, texture_t normal)
{
  return (material_t) {
    .albedo = albedo,
    .normal = normal
  };
}

void material_bind(material_t mat)
{
  texture_bind(mat.albedo, GL_TEXTURE_2D, 0);
  texture_bind(mat.normal, GL_TEXTURE_2D, 1);
}
