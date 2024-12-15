#ifndef LIGHTING_GLSL
#define LIGHTING_GLSL

#define MAX_LIGHTS 4

struct ubo_light {
  vec3 position;
  float pad1;
  vec3 radiance;
  float intensity;
};

layout (std140) uniform ubo_lighting {
  ubo_light lights[MAX_LIGHTS];
};

#endif
