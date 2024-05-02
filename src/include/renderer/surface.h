#ifndef SURFACE_H
#define SURFACE_H

typedef struct {
  texture_t albedo;
  texture_t normal;
} material_t;

typedef struct surface_s *surface_t;

void surface_shader_source(shaderdata_t sd, const char *path);

#endif
