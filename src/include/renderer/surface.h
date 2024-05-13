#ifndef SURFACE_H
#define SURFACE_H

#include <renderer/texture.h>
#include <renderer/target.h>
#include <renderer/shader.h>

typedef struct {
  texture_t albedo;
  texture_t normal;
} material_t;

material_t material_create(texture_t albedo, texture_t normal);
void material_bind(material_t mat);

typedef struct surface_s *surface_t;

void surface_shader_source(shaderdata_t sd, const char *path);

surface_t surface_create();
void surface_in(surface_t sf, const char *name, GLuint type, GLuint texture);
void surface_shader_load(surface_t sf, shaderdata_t sd, const char *path);
void surface_bind(surface_t sf);
void surface_destroy(surface_t sf);
shader_t surface_get_shader();

#endif
