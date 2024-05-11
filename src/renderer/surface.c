#include <renderer/surface.h>
#include <renderer/camera.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_CHANNEL 8

typedef struct {
  GLuint type;
  texture_t texture;
  char *name;
} channel_t;

struct surface_s {
  GLuint shader;
  channel_t channel[MAX_CHANNEL];
  int num_channel;
};

surface_t surface_create()
{
  surface_t sf = malloc(sizeof(*sf));
  sf->shader = GL_INVALID_VALUE;
  sf->num_channel = 0;
  return sf;
}

void surface_shader_load(surface_t sf, shaderdata_t sd, const char *path)
{
  camera_shader_import(sd);
  shaderdata_text(sd, "#define PLANAR_UV\n", SD_VERT);
  shaderdata_source(sd, "assets/shader/surface/surface.vert", SD_VERT);
  shaderdata_source(sd, path, SD_FRAG);
  shaderdata_text(sd, "in vec2 vs_uv;", SD_FRAG);
  shaderdata_text(sd, "in vec3 vs_pos;", SD_FRAG);
  shaderdata_text(sd, "in mat3 vs_TBN;", SD_FRAG);
  shaderdata_text(sd, "uniform sampler2D u_albedo;", SD_FRAG);
  shaderdata_text(sd, "uniform sampler2D u_normal;", SD_FRAG);
  shaderdata_text(
    sd,
    "void main() {\
      vec3 N = normalize(vs_TBN * (texture(u_normal, vs_uv).rgb * 2.0 - 1.0));\
      surface_shade(vs_pos, vs_uv, N, u_albedo);\
    }",
    SD_FRAG
  );
  
  sf->shader = shader_load(sd);
  
  camera_shader_attach(sf->shader);
  glUniform1i(glGetUniformLocation(sf->shader, "u_albedo"), 0); 
  glUniform1i(glGetUniformLocation(sf->shader, "u_normal"), 1);
  
  for (int i = 0; i < sf->num_channel; i++) {
    glUniform1i(glGetUniformLocation(sf->shader, sf->channel[i].name), 2 + i);
  }
}

void surface_bind(surface_t sf, const char *name, GLuint type, GLuint texture)
{
  int channel = sf->num_channel++;
  
  if (channel >= MAX_CHANNEL) {
    return;
  }
  
  sf->channel[channel].name = strdup(name);
  sf->channel[channel].type = type;
  sf->channel[channel].texture = texture;
}

void surface_use(surface_t sf)
{
  shader_bind(sf->shader);
  
  for (int i = 0; i < sf->num_channel; i++) {
    texture_bind(sf->channel[i].texture, sf->channel[i].type, 2 + i); 
  }
}

void surface_destroy(surface_t sf)
{
  shader_destroy(sf->shader);
  
  for (int i = 0; i < sf->num_channel; i++) {
    free(sf->channel[i].name);
  }
  
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
