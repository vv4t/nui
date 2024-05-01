#ifndef FRAME_H
#define FRAME_H

#include <renderer/shader.h>
#include <renderer/target.h>
#include <renderer/texture.h>

typedef struct framebuffer_s *framebuffer_t;

void frame_init();
void frame_shader_source(shaderdata_t sd, const char *path);
shader_t frame_shader_load(const char *path);
void frame_draw(shader_t shader);

framebuffer_t framebuffer_create(int width, int height);
void framebuffer_begin(framebuffer_t fb);
void framebuffer_end();
void framebuffer_bind(framebuffer_t fb, int channel, GLuint type, texture_t texture);
void framebuffer_update(framebuffer_t fb, shader_t shader);
void framebuffer_draw(framebuffer_t fb, shader_t shader);
void framebuffer_destroy(framebuffer_t fb);
texture_t framebuffer_get_texture(framebuffer_t fb);
target_t framebuffer_get_target(framebuffer_t fb);

#endif
