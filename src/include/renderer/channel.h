#ifndef CHANNEL_H
#define CHANNEL_H

#include <renderer/shader.h>
#include <renderer/texture.h>
#include <GL/glew.h>

typedef struct channel_s *channel_t;

channel_t channel_create();
void channel_in(channel_t ch, const char *name, GLuint type, texture_t texture);
void channel_disable(channel_t ch, int channel);
void channel_shader_attach(shader_t shader, channel_t ch);
void channel_bind(channel_t ch);
void channel_destroy(channel_t ch);
channel_t channel_copy(channel_t src);

#endif
