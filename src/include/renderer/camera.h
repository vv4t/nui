#ifndef CAMERA_H
#define CAMERA_H

#include <math3d.h>
#include <renderer/shader.h>
#include <renderer/shaderdata.h>

void camera_init();
void camera_orthogonal(float w, float h);
void camera_perspective(float w, float h, float n, float f);
void camera_move(vector position, vector rotation);
void camera_update(matrix m);
void camera_shader_attach(shader_t shader);
void camera_shader_import(shaderdata_t sd);
void camera_deinit();

#endif
