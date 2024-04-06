#version 300 es

layout(location = 0) in vec3 v_p;

uniform mat4 u_mvp;

void main()
{
  gl_Position = u_mvp * vec4(v_p, 1.0);
}
