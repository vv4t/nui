layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_tangent;
layout(location = 2) in vec3 v_bitangent;
layout(location = 3) in vec3 v_normal;
layout(location = 4) in vec2 v_uv;

layout (std140) uniform ubo_matrices {
  mat4  mvp;
  mat4  model;
};

out vec2  vs_uv;
out float vs_depth;

void main()
{
  vs_uv = v_uv;
  gl_Position = mvp * vec4(v_pos, 1.0);
  vs_depth = gl_Position.z;
}
