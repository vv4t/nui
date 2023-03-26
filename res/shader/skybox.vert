#version 300 es

layout (location = 0) in vec3 v_pos;

out vec3 vs_uv;

uniform mat4 u_mvp;

void main()
{
  vs_uv = v_pos;
  gl_Position = u_mvp * vec4(v_pos, 1.0);
}
