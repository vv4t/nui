#version 300 es

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_tangent;
layout(location = 2) in vec3 v_bitangent;
layout(location = 3) in vec3 v_normal;
layout(location = 4) in vec2 v_uv;

layout (std140) uniform ubo_matrices {
  mat4  mvp;
  mat4  model;
  vec3  view_pos;
  float pad[1];
};

out vec2 vs_uv;
out vec3 vs_pos;
out vec3 vs_normal;
out mat3 vs_TBN;

void main() {
  vec3 tangent = vec3(model * vec4(v_tangent, 1.0));
  vec3 bitangent = vec3(model * vec4(v_bitangent, 1.0));
  vec3 normal = vec3(model * vec4(v_normal, 1.0));
  
  vs_TBN = mat3(tangent, bitangent, normal);
  
  vs_pos = vec3(model * vec4(v_pos, 1.0));
  vs_uv = v_uv;
  vs_normal = normal;
  
  gl_Position = mvp * vec4(v_pos, 1.0);
}
