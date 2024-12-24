#ifndef CAMERA_GLSL
#define CAMERA_GLSL

layout (std140) uniform ubo_camera {
  mat4 MVP;
  mat4 view_project;
  mat4 view;
  mat4 model;
  vec3 view_pos;
};

#endif
