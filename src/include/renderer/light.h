#ifndef LIGHT_H
#define LIGHT_H

#define POINT_MAX 8

#include <math3d.h>
#include <renderer/shader.h>

typedef int point_t;

void light_init();
point_t light_add_point(vector position, vector color);
void light_update_point(point_t point, vector position, vector color);
void light_shader_import(shaderdata_t sd);
void light_shader_attach(shader_t shader);
void light_deinit();

#endif
