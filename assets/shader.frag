#version 300 es
precision mediump float;

in vec2 vs_uv;
uniform sampler2D sampler;
out vec4 frag_color;

void main()
{
  frag_color = texture(sampler, vs_uv);
}
