#ifndef LIGHT_H
#define LIGHT_H

#include <math3d.h>
#include <renderer/shader.h>

void light_init();
void light_add_point(vector position, vector color);
void light_shader_import(shaderdata_t sd);
void light_shader_attach(shader_t shader);
void light_deinit();

#endif
