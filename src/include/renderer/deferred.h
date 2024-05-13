#ifndef DEFERRED_H
#define DEFERRED_H

#include <renderer/texture.h>
#include <renderer/shader.h>

typedef struct gbuffer_s *gbuffer_t;

void gbuffer_init();
void gbuffer_deinit();

gbuffer_t gbuffer_create(texture_t albedo, texture_t position, texture_t normal, texture_t depth);
void gbuffer_bind(gbuffer_t gb);
void gbuffer_unbind();
void gbuffer_destroy(gbuffer_t gb);

typedef struct deferred_s *deferred_t;

deferred_t deferred_create(gbuffer_t gb);
void deferred_shader_load(deferred_t df, shaderdata_t sd, const char *path);
void deferred_in(deferred_t df, const char *name, GLuint type, GLuint texture);
void deferred_display(deferred_t df);
void deferred_destroy(deferred_t df);
shader_t deferred_get_shader();

#endif
