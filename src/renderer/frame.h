#ifndef FRAME_H
#define FRAME_H

#include "../gl/gl.h"
#include "../gl/shader.h"

void frame_init(int width, int height);
void frame_begin(int frame_num);
void frame_end();
void frame_draw(GLuint shader, int frame_num);
void frame_shader_source(shader_setup_t *shader_setup, const char *name);
bool frame_shader_load(GLuint *shader, const char *name);
GLuint frame_get_fbo(int frame_num);

#endif
