out vec4 frag_color;

in vec2 vs_uv;

uniform sampler2D u_color;

void main()
{
  frag_color = texture(u_color, vs_uv);
}
