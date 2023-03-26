#version 300 es
precision mediump float;

in vec3 vs_pos;

out float frag_depth;

layout (std140) uniform ubo_matrices {
  mat4  mvp;
  mat4  model;
  vec3  view_pos;
  float pad[1];
};

void main()
{
  frag_depth = length(vs_pos - view_pos);
}
