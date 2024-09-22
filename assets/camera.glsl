#ifndef CAMERA_GLSL
#define CAMERA_GLSL

layout (std140) uniform ubo_camera {
  mat4 camera_MVP;
};

#endif
