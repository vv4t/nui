#ifndef FRAME_H
#define FRAME_H

#include <renderer/shader.h>
#include <renderer/target.h>
#include <renderer/texture.h>

typedef struct frame_s *frame_t;

void frame_init();
void frame_shader_source(shaderdata_t sd, const char *path);
shader_t frame_shader_load(const char *path);

frame_t frame_create(int width, int height, texture_t depth_buffer);
void frame_begin(frame_t fr);
void frame_end();
void frame_bind(frame_t fr, int channel, GLuint type, texture_t texture);
void frame_update(frame_t fr, shader_t shader);
void frame_draw(frame_t fr, shader_t shader);
void frame_destroy(frame_t fr);
texture_t frame_get_texture(frame_t fr);

#endif
