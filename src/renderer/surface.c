#include <renderer/surface.h>
#include <renderer/camera.h>
#include <stdlib.h>

#define MAX_CHANNEL 8

typedef struct {
  GLuint type;
  texture_t texture;
} channel_t;

struct surface_s {
  target_t target;
  channel_t channel[MAX_CHANNEL];
};

surface_t surface_create()
{
  surface_t sf = malloc(sizeof(*sf));
  sf->target = GL_INVALID_VALUE;
  
  for (int i = 0; i < MAX_CHANNEL; i++) {
    sf->channel[i].texture = GL_INVALID_VALUE;
  }
  
  return sf;
}

shader_t surface_shader_load(surface_t sf, const char *path)
{
  shaderdata_t sd = shaderdata_create();
  surface_shader_source(sd, path);
  shader_t shader = shader_load(sd);
  surface_shader_attach(shader, sf);
  shaderdata_destroy(sd);
  return shader;
}

void surface_shader_source(shaderdata_t sd, const char *path)
{
  camera_shader_import(sd);
  shaderdata_text(sd, "#define PLANAR_UV\n", SD_VERT);
  shaderdata_source(sd, "assets/shader/vertex/surface.vert", SD_VERT);
  shaderdata_source(sd, path, SD_FRAG);
  shaderdata_text(sd, "in vec2 vs_uv;", SD_FRAG);
  shaderdata_text(sd, "in vec3 vs_pos;", SD_FRAG);
  shaderdata_text(sd, "in mat3 vs_TBN;", SD_FRAG);
  shaderdata_text(sd, "uniform sampler2D u_albedo;", SD_FRAG);
  shaderdata_text(sd, "uniform sampler2D u_normal;", SD_FRAG);
  shaderdata_text(
    sd,
    "void main() {\
      vec3 normal = normalize(vs_TBN * (texture(u_normal, vs_uv).rgb * 2.0 - 1.0));\
      surface_shade(vs_pos, vs_uv, normal, u_albedo);\
    }",
    SD_FRAG
  );
}

void surface_shader_attach(shader_t shader, surface_t sf)
{
  camera_shader_attach(shader);
  glUniform1i(glGetUniformLocation(shader, "u_albedo"), 0); 
  glUniform1i(glGetUniformLocation(shader, "u_normal"), 1); 
}

void surface_bind(surface_t sf, shader_t shader)
{
  shader_bind(shader);
  
  if (sf->target != GL_INVALID_VALUE) {
    target_bind(sf->target);
  }
}

void surface_unbind()
{
  target_unbind();
}

void surface_destroy(surface_t sf)
{
  free(sf);
}
