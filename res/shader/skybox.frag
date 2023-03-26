#version 300 es
precision mediump float;

out vec4 frag_color;

in vec3 vs_uv;

uniform samplerCube u_skybox;

void main()
{
  frag_color = texture(u_skybox, vs_uv);
}
