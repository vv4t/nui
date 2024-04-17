out vec4 frag_color;

in vec2 vs_uv;

uniform sampler2D sampler;

void main()
{
  frag_color = texture(sampler, vs_uv);
}
