#ifndef SURFACE_H
#define SURFACE_H

#include <renderer/texture.h>
#include <renderer/target.h>
#include <renderer/shader.h>

typedef struct {
  texture_t albedo;
} material_t;

material_t material_create();
void material_bind(material_t mtl);

typedef struct surface_s *surface_t;

void surface_shader_source(shaderdata_t sd, const char *path);

surface_t surface_create();
shader_t surface_shader_load(surface_t sf, const char *path);
void surface_add_channel(surface_t sf, const char *name, GLuint type, GLuint texture);
void surface_shader_source(shaderdata_t sd, const char *path);
void surface_shader_attach(shader_t shader, surface_t sf);
void surface_bind(surface_t sf, shader_t shader);
void surface_unbind();
void surface_destroy(surface_t sf);

#endif
